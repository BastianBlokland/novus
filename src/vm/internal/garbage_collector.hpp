#pragma once
#include "internal/executor_registry.hpp"
#include <thread>

namespace vm::internal {

class Allocator;

class GarbageCollector final {
public:
  GarbageCollector(Allocator* allocator, ExecutorRegistry* execRegistry) noexcept;
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

  Allocator* m_allocator;
  ExecutorRegistry* m_execRegistry;

  std::thread m_collectorThread;
  RequestType m_requestType;
  std::mutex m_requestMutex;
  std::condition_variable m_requestCondVar;

  auto request(RequestType type) noexcept -> void;
  auto collectorLoop() noexcept -> void;
  auto collect() noexcept -> void;
};

} // namespace vm::internal
