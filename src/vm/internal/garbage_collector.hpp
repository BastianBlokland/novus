#pragma once
#include "internal/executor_registry.hpp"
#include <condition_variable>
#include <thread>
#include <vector>

namespace vm::internal {

class RefAllocator;

const auto gcMinIntervalSeconds   = 10;
const auto initialGcMarkQueueSize = 1024;

class GarbageCollector final {
public:
  GarbageCollector(RefAllocator* refAlloc, ExecutorRegistry* execRegistry) noexcept;
  GarbageCollector(const GarbageCollector& rhs) = delete;
  GarbageCollector(GarbageCollector&& rhs)      = delete;
  ~GarbageCollector() noexcept;

  auto operator=(const GarbageCollector& rhs) -> GarbageCollector& = delete;
  auto operator=(GarbageCollector&& rhs) -> GarbageCollector& = delete;

  auto requestCollection() noexcept -> void;
  auto terminateCollector() noexcept -> void;

private:
  enum class RequestType : int {
    None      = 0,
    Collect   = 1,
    Terminate = 2,
  };

  RefAllocator* m_refAlloc;
  ExecutorRegistry* m_execRegistry;
  std::vector<Ref*> m_markQueue;

  std::thread m_collectorThread;
  RequestType m_requestType;
  std::mutex m_requestMutex;
  std::condition_variable m_requestCondVar;

  auto request(RequestType type) noexcept -> void;
  auto collectorLoop() noexcept -> void;

  auto collect() noexcept -> void;
  auto populateMarkQueue() noexcept -> void;
  auto populateMarkQueue(BasicStack* stack) noexcept -> void;
  auto mark() noexcept -> void;
  auto sweep(Ref* head) noexcept -> void;
};

} // namespace vm::internal
