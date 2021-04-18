#pragma once
#include "internal/executor_registry.hpp"
#include "internal/ref_alloc_observer.hpp"
#include <condition_variable>
#include <vector>

namespace vm::internal {

class RefAllocator;

const auto gcByteInterval            = 100U * 1024U * 1024U; // 100 MiB
const auto gcMinIntervalMilliseconds = 5000U;
const auto initialGcMarkQueueSize    = 1024U;

enum GarbageCollectFlags {
  GcCollectNormal        = 0,
  GcCollectBlockingSweep = 1 << 0,
};

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
  using CollectionId = uint64_t;

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

  auto requestCollection(GarbageCollectFlags flags) noexcept -> CollectionId;

  /* Request a new collection and block until its finished.
   * NOTE: When calling this from an executor make sure to mark your executor as paused before
   * calling this, otherwise this will deadlock when it tries to pause your executor.
   */
  auto collectNow(GarbageCollectFlags flags) noexcept -> void;

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
  GarbageCollectFlags m_requestCollectFlags;
  std::mutex m_requestMutex;
  std::condition_variable m_requestCondVar;
  std::atomic<CollectionId> m_collectionStarted;
  std::atomic<CollectionId> m_collectionFinished;

  auto notifyAlloc(unsigned int size) noexcept -> void override;

  auto collectorLoop() noexcept -> void;

  auto collect(GarbageCollectFlags flags) noexcept -> void;
  auto populateMarkQueue() noexcept -> void;
  auto populateMarkQueue(BasicStack* stack) noexcept -> void;
  auto mark() noexcept -> void;
  auto sweep(Ref* head) noexcept -> void;
};

} // namespace vm::internal
