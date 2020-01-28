#pragma once
#include "internal/eval_stack.hpp"
#include "internal/string_utilities.hpp"
#include "vm/exceptions/assert_failed.hpp"
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
  case vm::PCallCode::GetEnvArg: {
    auto* res = iface.getEnvArg(evalStack.pop().getInt());
    if (res == nullptr) {
      evalStack.push(internal::emptyString(allocator));
    } else {
      evalStack.push(internal::toString(allocator, res));
    }
    return;
  }
  case vm::PCallCode::GetEnvArgCount: {
    evalStack.push(internal::intValue(iface.getEnvArgCount()));
    return;
  }
  case vm::PCallCode::GetEnvVar: {
    auto* nameStrRef = getStringRef(evalStack.pop());
    auto* res        = iface.getEnvVar(nameStrRef->getDataPtr());
    if (res == nullptr) {
      evalStack.push(internal::emptyString(allocator));
    } else {
      evalStack.push(internal::toString(allocator, res));
    }
    return;
  }
  case vm::PCallCode::Sleep:
    std::this_thread::sleep_for(std::chrono::milliseconds(evalStack.peek().getInt()));
    return;
  case vm::PCallCode::Assert: {
    auto* msg = getStringRef(evalStack.pop());
    auto cond = evalStack.peek().getInt();
    if (cond == 0) {
      throw exceptions::AssertFailed{"Assert failed: " +
                                     std::string{msg->getDataPtr(), msg->getSize()}};
    }
    return;
  }
  }
  throw exceptions::InvalidAssembly{};
}

} // namespace vm::internal
