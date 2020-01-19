#pragma once
#include "internal/eval_stack.hpp"
#include "internal/string_utilities.hpp"
#include "vm/exceptions/invalid_assembly.hpp"
#include "vm/pcall_code.hpp"
#include <chrono>
#include <thread>

namespace vm::internal {

const auto static newl = '\n';

template <typename EvalStack, typename PlatformInterface>
auto inline pcall(
    EvalStack& evalStack, Allocator* allocator, PlatformInterface& iface, PCallCode code) -> void {

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
  case vm::PCallCode::ReadStringLine: {
    std::string line = {};
    while (true) {
      const auto c = iface.read();
      if (c == '\0' || c == '\n') {
        break;
      }
      line += c;
    }
    evalStack.push(internal::toString(allocator, line));
    return;
  }
  case vm::PCallCode::Sleep:
    std::this_thread::sleep_for(std::chrono::milliseconds(evalStack.peek().getInt()));
    return;
  }
  throw exceptions::InvalidAssembly{};
}

} // namespace vm::internal
