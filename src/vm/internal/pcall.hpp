#pragma once
#include "internal/eval_stack.hpp"
#include "vm/exceptions/invalid_assembly.hpp"
#include "vm/pcall_code.hpp"

namespace vm::internal {

const auto static newl = '\n';

template <typename EvalStack, typename PlatformInterface>
auto inline pcall(EvalStack& evalStack, PlatformInterface& iface, PCallCode code) -> void {
  switch (code) {
  case vm::PCallCode::Print: {
    auto* strRef = getStringRef(evalStack.peek());
    iface.print(strRef->getDataPtr(), strRef->getSize());
    return;
  }
  case vm::PCallCode::PrintLine: {
    auto* strRef = getStringRef(evalStack.peek());
    iface.print(strRef->getDataPtr(), strRef->getSize());
    iface.print(&newl, 1);
    return;
  }
  }
  throw exceptions::InvalidAssembly{};
}

} // namespace vm::internal
