#pragma once
#include "internal/executor_registry.hpp"
#include "internal/ref_alloc_observer.hpp"
#include <condition_variable>
#include <vector>

namespace vm::internal {

class RefAllocator;

const auto gcByteInterval         = 100U * 1024U * 1024U; // 100 MiB
const auto gcMinIntervalSeconds   = 10U;
const auto initialGcMarkQueueSize = 1024U;

// Garbage collector is responsible for freeing unused references. It uses allocated bytes and
// elapsed time as heuristics to decide when to run a collection pass.
//
// When collecting garbage it performs these steps:
// * Wake up the collector thread.
// * Pause all executors ('Stop the world').
// * Mark all used objects on the stacks of all executors.
// * Resume all executors ('Resume the world').
// * Remove all unused references ('Sweep').
// * Put the collector thread to sleep.
//
class GarbageCollector final : public RefAllocObserver {
public:
  enum class CollectorStartResult {
    Success = 0,
    Failure = 1,
  };

  GarbageCollector(RefAllocator* refAlloc, ExecutorRegistry* execRegistry) noexcept;
  GarbageCollector(const GarbageCollector& rhs) = delete;
  GarbageCollector(GarbageCollector&& rhs)      = delete;
  ~GarbageCollector() noexcept;

  auto operator=(const GarbageCollector& rhs) -> GarbageCollector& = delete;
  auto operator=(GarbageCollector&& rhs) -> GarbageCollector& = delete;

  [[nodiscard]] auto startCollector() noexcept -> CollectorStartResult;

  auto requestCollection() noexcept -> void;

  auto terminateCollector() noexcept -> void;

private:
  enum class RequestType : int {
    None      = 0,
    Collect   = 1,
    Terminate = 2,
  };

  enum class CollectorStatus : int {
    NotRunning = 0,
    Running    = 1,
    Terminated = 2,
  };

  RefAllocator* m_refAlloc;
  ExecutorRegistry* m_execRegistry;
  std::vector<Ref*> m_markQueue;
  std::atomic<int> m_bytesUntilNextCollection;

  std::atomic<CollectorStatus> m_collectorStatus;
  RequestType m_requestType;
  std::mutex m_requestMutex;
  std::condition_variable m_requestCondVar;

  auto notifyAlloc(unsigned int size) noexcept -> void override;

  auto request(RequestType type) noexcept -> void;
  auto collectorLoop() noexcept -> void;

  auto collect() noexcept -> void;
  auto populateMarkQueue() noexcept -> void;
  auto populateMarkQueue(BasicStack* stack) noexcept -> void;
  auto mark() noexcept -> void;
  auto sweep(Ref* head) noexcept -> void;
};

} // namespace vm::internal
