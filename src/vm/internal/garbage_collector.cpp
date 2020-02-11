#include "internal/garbage_collector.hpp"
#include "internal/allocator.hpp"
#include "internal/ref_future.hpp"
#include "internal/ref_struct.hpp"
#include <condition_variable>
#include <mutex>

namespace vm::internal {

GarbageCollector::GarbageCollector(Allocator* allocator, ExecutorRegistry* execRegistry) noexcept :
    m_allocator{allocator}, m_execRegistry{execRegistry}, m_requestType{RequestType::None} {

  // Start the garbage-collector thread.
  m_collectorThread = std::thread(&GarbageCollector::collectorLoop, this);
}

GarbageCollector::~GarbageCollector() noexcept { terminateCollector(); }

auto GarbageCollector::requestCollection() noexcept -> void {
  {
    std::lock_guard<std::mutex> lk(m_requestMutex);
    if (m_requestType == RequestType::None) {
      m_requestType = RequestType::Collect;
    }
  }
  m_requestCondVar.notify_one();
}

auto GarbageCollector::terminateCollector() noexcept -> void {
  // If the collector-thread is already joined then early out.
  if (!m_collectorThread.joinable()) {
    return;
  }
  {
    std::lock_guard<std::mutex> lk(m_requestMutex);
    m_requestType = RequestType::Terminate;
  }
  m_requestCondVar.notify_one();
  m_collectorThread.join();
}

auto GarbageCollector::collectorLoop() noexcept -> void {
  while (true) {
    // Wait for a request.
    {
      std::unique_lock<std::mutex> lk(m_requestMutex);
      m_requestCondVar.wait(lk, [this]() { return m_requestType != RequestType::None; });
      if (m_requestType == RequestType::Terminate) {
        return;
      }
      m_requestType = RequestType::None;
    }

    // Collect garbage.
    collect();
  }
}

auto mark(ExecutorRegistry* execRegistry) noexcept -> void;
auto markStack(BasicStack* stack) noexcept -> void;
auto markRef(Ref* ref) noexcept -> void;
auto markValue(const Value& val) noexcept -> void;
auto sweep(Allocator* allocator) noexcept -> void;

auto GarbageCollector::collect() noexcept -> void {
  // Pause all executors. This makes sure that we are free to inspect the stacks of the allocators
  // and no new allocations are being made.
  m_execRegistry->pauseExecutors(); // Will block until all executors have paused.

  // Mark all references that are reachable from the stacks of the executors.
  mark(m_execRegistry);

  // Remove all non-marked allocations.
  sweep(m_allocator);

  m_execRegistry->resumeExecutors();
}

inline auto mark(ExecutorRegistry* execRegistry) noexcept -> void {
  auto* execHandle = execRegistry->getHeadExecutor();
  while (execHandle) {
    markStack(execHandle->getStack());
    execHandle = execHandle->getNext();
  }
}

inline auto markStack(BasicStack* stack) noexcept -> void {
  for (auto* sp = stack->getBottom(); sp != stack->getNext(); ++sp) {
    assert(sp < stack->getNext());

    markValue(*sp);
  }
}

inline auto markValue(const Value& val) noexcept -> void {
  if (val.isRef()) {
    markRef(val.getRef());
  }
}

auto markRef(Ref* ref) noexcept -> void {
  ref->setFlag<RefFlags::GcMarked>();
  switch (ref->getKind()) {
  case RefKind::String:
    break;
  case RefKind::Struct: {
    auto* s = downcastRef<StructRef>(ref);
    for (auto* fP = s->getFieldsBegin(); fP != s->getFieldsEnd(); ++fP) {
      markValue(*fP);
    }
  } break;
  case RefKind::Future: {
    auto* f = downcastRef<FutureRef>(ref);
    markValue(f->getResult());
  } break;
  }
}

inline auto sweep(Allocator* allocator) noexcept -> void {
  Ref* prevRef = nullptr;
  Ref* ref     = allocator->getHeadAlloc();
  while (ref) {
    if (ref->hasFlag<RefFlags::GcMarked>()) {
      // Still reachable.
      ref->unsetFlag<RefFlags::GcMarked>();
      prevRef = ref;
      ref     = allocator->getNextAlloc(ref);
    } else {
      // No longer reachable.
      if (prevRef == nullptr) {
        ref = allocator->freeHead();
      } else {
        ref = allocator->freeNext(prevRef);
      }
    }
  }
}

} // namespace vm::internal
