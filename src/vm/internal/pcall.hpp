#pragma once
#include "internal/eval_stack.hpp"
#include "vm/exceptions/invalid_assembly.hpp"
#include "vm/pcall_code.hpp"

namespace vm::internal {

const auto static newl = '\n';

template <typename EvalStack, typename PlatformInterface>
auto inline pcall(EvalStack& evalStack, PlatformInterface& iface, PCallCode code) -> void {
  switch (code) {
  case vm::PCallCode::PrintChar: {
    auto c = static_cast<char>(evalStack.peek().getInt());
    iface.print(&c, 1);
    return;
  }
  case vm::PCallCode::PrintString: {
    auto* strRef = getStringRef(evalStack.peek());
    iface.print(strRef->getDataPtr(), strRef->getSize());
    return;
  }
  case vm::PCallCode::PrintStringLine: {
    auto* strRef = getStringRef(evalStack.peek());
    iface.print(strRef->getDataPtr(), strRef->getSize());
    iface.print(&newl, 1);
    return;
  }
  case vm::PCallCode::ReadChar: {
    evalStack.push(internal::intValue(iface.read()));
    return;
  }
  }
  throw exceptions::InvalidAssembly{};
}

} // namespace vm::internal
