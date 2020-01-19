#pragma once
#include "internal/eval_stack.hpp"
#include "vm/pcall_code.hpp"
#include "vm/platform/interface.hpp"

namespace vm::internal {

template <typename EvalStack>
auto inline pcall(EvalStack& evalStack, platform::Interface* iface, PCallCode code) -> void {
  switch (code) {
  case vm::PCallCode::Print: {
    auto* strRef = getStringRef(evalStack.peek());
    iface->print(strRef->getDataPtr(), strRef->getSize());
  } break;
  }
}

} // namespace vm::internal
