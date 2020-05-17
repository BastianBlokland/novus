#pragma once

namespace vm::internal {

// Interface to be notified about new reference allocations.
class RefAllocObserver {
public:
  virtual auto notifyAlloc(unsigned int size) noexcept -> void = 0;
};

} // namespace vm::internal
