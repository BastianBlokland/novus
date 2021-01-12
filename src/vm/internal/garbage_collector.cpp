#include "internal/garbage_collector.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_future.hpp"
#include "internal/ref_stream_process.hpp"
#include "internal/ref_string_link.hpp"
#include "internal/ref_struct.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace vm::internal {

// To avoid too much contention on the 'm_bytesUntilNextCollection' atomic we first fill a
// thread-static counter before decreasing the atomic.
const static auto bytesAllocThreadAccumMax = 1024U * 1024U; // 1 MiB
thread_local static unsigned int bytesAllocThreadAccum;

GarbageCollector::GarbageCollector(RefAllocator* refAlloc, ExecutorRegistry* execReg) noexcept :
    m_refAlloc{refAlloc}, m_execRegistry{execReg}, m_requestType{RequestType::None} {

  // Subscribe to allocation notifications, we can use these to decide when to run a collection.
  refAlloc->subscribe(this);

  m_markQueue.reserve(initialGcMarkQueueSize);

  // Start the garbage-collector thread.
  m_collectorThread = std::thread(&GarbageCollector::collectorLoop, this);
}

GarbageCollector::~GarbageCollector() noexcept { terminateCollector(); }

auto GarbageCollector::requestCollection() noexcept -> void {
  {
    std::lock_guard<std::mutex> lk(m_requestMutex);
    if (likely(m_requestType == RequestType::None)) {
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

auto GarbageCollector::notifyAlloc(unsigned int size) noexcept -> void {
  // Increase the thread-static counter.
  bytesAllocThreadAccum += size;
  if (bytesAllocThreadAccum > bytesAllocThreadAccumMax) {

    // Decrease the bytesUntilNextCollection atomic.
    if (m_bytesUntilNextCollection.fetch_sub(bytesAllocThreadAccum, std::memory_order_acq_rel) <
        0) {
      m_bytesUntilNextCollection.store(gcByteInterval, std::memory_order_release);
      requestCollection();
    }

    bytesAllocThreadAccum = 0;
  }
}

auto GarbageCollector::collectorLoop() noexcept -> void {
  while (true) {
    // Wait for a request (or timout for a minimum gc interval).
    {
      std::unique_lock<std::mutex> lk(m_requestMutex);
      m_requestCondVar.wait_for(lk, std::chrono::seconds(gcMinIntervalSeconds), [this]() {
        return m_requestType != RequestType::None;
      });
      if (unlikely(m_requestType == RequestType::Terminate)) {
        return;
      }

      // Reset the bytes counter.
      m_bytesUntilNextCollection.store(gcByteInterval, std::memory_order_release);

      m_requestType = RequestType::None;
    }

    // Collect garbage.
    collect();
  }
}

auto GarbageCollector::collect() noexcept -> void {
  // Pause all executors. This makes sure that we are free to inspect the stacks of the allocators
  // and no new allocations are being made.
  m_execRegistry->pauseExecutors(); // Will block until all executors have paused.

  // Populate mark-queue with the references from the stacks of the executors.
  populateMarkQueue();

  // Mark all references in the mark-queue.
  mark();

  // Get the head ref to start the sweep from.
  Ref* sweepHead = m_refAlloc->getHeadAlloc();

  // Resume the executors as the sweeping can run concurrently with the program.
  m_execRegistry->resumeExecutors();

  // Remove all non-marked allocations.
  sweep(sweepHead);
}

auto GarbageCollector::populateMarkQueue() noexcept -> void {
  // Go through all the executors and process their stacks.
  auto* execHandle = m_execRegistry->getHeadExecutor();
  while (execHandle) {
    populateMarkQueue(execHandle->getStack());
    execHandle = execHandle->getNext();
  }
}

auto GarbageCollector::populateMarkQueue(BasicStack* stack) noexcept -> void {
  // Add all references on the stack to the mark-queue.
  for (auto* sp = stack->getBottom(); sp != stack->getNext(); ++sp) {
    assert(sp < stack->getNext());
    if (sp->isRef()) {
      auto* ref = sp->getRef();
      if (ref != nullptr) {
        m_markQueue.push_back(ref);
      }
    }
  }
}

auto GarbageCollector::mark() noexcept -> void {
  while (!m_markQueue.empty()) {
    // Take a reference from the queue.
    Ref* cur = m_markQueue.back();
    m_markQueue.pop_back();

    // If its allready marked then we ignore it.
    if (cur->hasFlag<RefFlags::GcMarked>()) {
      continue;
    }

    // Mark it.
    cur->setFlag<RefFlags::GcMarked>();

    // Push any child references it has to the queue.
    switch (cur->getKind()) {
    case RefKind::Struct: {
      auto* s = downcastRef<StructRef>(cur);
      for (auto* fP = s->getFieldsBegin(); fP != s->getFieldsEnd(); ++fP) {
        if (fP->isRef()) {
          auto* ref = fP->getRef();
          if (ref != nullptr) {
            m_markQueue.push_back(ref);
          }
        }
      }
    } break;
    case RefKind::Future: {
      auto* f  = downcastRef<FutureRef>(cur);
      auto res = f->getResult();
      if (res.isRef()) {
        auto* ref = res.getRef();
        if (ref != nullptr) {
          m_markQueue.push_back(ref);
        }
      }
    } break;
    case RefKind::StringLink: {
      auto* l = downcastRef<StringLinkRef>(cur);
      if (l->isCollapsed()) {
        m_markQueue.push_back(l->getCollapsed());
        // If a collapsed representation has been computed we can safely discard the 'link' to the
        // rest of the chain.
        l->clearLink();
      } else {
        assert(l->getPrev() != nullptr);
        m_markQueue.push_back(l->getPrev());
        if (l->getVal().isRef()) {
          auto* valRef = l->getVal().getRef();
          assert(valRef != nullptr);
          m_markQueue.push_back(valRef);
        }
      }
    } break;
    case RefKind::StreamProcess:
      m_markQueue.push_back(downcastRef<ProcessStreamRef>(cur)->getProcess());
      break;
    case RefKind::Atomic:
    case RefKind::String:
    case RefKind::Long:
    case RefKind::StreamFile:
    case RefKind::StreamConsole:
    case RefKind::StreamTcp:
    case RefKind::Process:
      break;
    }
  }
}

auto GarbageCollector::sweep(Ref* head) noexcept -> void {
  if (unlikely(head == nullptr)) {
    return;
  }

  // Walks the list of allocations, if its marked then its unmarked and if its not marked it is
  // deleted. This won't ever delete the head node, reason is that would require syncronization as
  // the running program might change the head.

  head->unsetFlag<RefFlags::GcMarked>();

  Ref* prev = head;
  Ref* cur  = m_refAlloc->getNextAlloc(head);
  while (cur) {
    if (cur->hasFlag<RefFlags::GcMarked>()) {
      // Still reachable.
      cur->unsetFlag<RefFlags::GcMarked>();
      prev = cur;
      cur  = m_refAlloc->getNextAlloc(cur);
    } else {
      // No longer reachable.
      cur = m_refAlloc->freeNext(prev);
    }
  }
}

} // namespace vm::internal
