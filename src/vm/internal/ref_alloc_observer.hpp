#pragma once

class RefAllocObserver {
public:
  virtual auto notifyAlloc(unsigned int size) noexcept -> void = 0;
};
