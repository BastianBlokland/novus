#pragma once
#include "internal/eval_stack.hpp"
#include "internal/string_utilities.hpp"
#include "vm/pcall_code.hpp"
#include "vm/result_code.hpp"
#include <chrono>
#include <thread>

namespace vm::internal {

const auto static newl = '\n';

template <typename EvalStack, typename PlatformInterface>
[[nodiscard]] auto inline pcall(
    EvalStack& evalStack, Allocator* allocator, PlatformInterface& iface, PCallCode code) noexcept
    -> ResultCode {

  switch (code) {
  case vm::PCallCode::ConWriteChar: {
    auto c = static_cast<char>(evalStack.peek().getInt());
    iface.conWrite(&c, 1);
    return ResultCode::Ok;
  }
  case vm::PCallCode::ConWriteString: {
    auto* strRef = getStringRef(evalStack.peek());
    iface.conWrite(strRef->getDataPtr(), strRef->getSize());
    return ResultCode::Ok;
  }
  case vm::PCallCode::ConWriteStringLine: {
    auto* strRef = getStringRef(evalStack.peek());
    iface.conWrite(strRef->getDataPtr(), strRef->getSize());
    iface.conWrite(&newl, 1);
    return ResultCode::Ok;
  }
  case vm::PCallCode::ConReadChar: {
    return evalStack.push(internal::intValue(iface.conRead()));
  }
  case vm::PCallCode::ConReadStringLine: {
    std::string line = {};
    while (true) {
      const auto c = iface.conRead();
      if (c == '\0' || c == '\n') {
        break;
      }
      line += c;
    }
    return evalStack.push(internal::toString(allocator, line));
  }
  case vm::PCallCode::GetEnvArg: {
    auto* res = iface.getEnvArg(evalStack.pop().getInt());
    if (res == nullptr) {
      return evalStack.push(internal::emptyString(allocator));
    }
    return evalStack.push(internal::toString(allocator, res));
  }
  case vm::PCallCode::GetEnvArgCount: {
    return evalStack.push(internal::intValue(iface.getEnvArgCount()));
  }
  case vm::PCallCode::GetEnvVar: {
    auto* nameStrRef = getStringRef(evalStack.pop());
    auto* res        = iface.getEnvVar(nameStrRef->getDataPtr());
    if (res == nullptr) {
      return evalStack.push(internal::emptyString(allocator));
    }
    return evalStack.push(internal::toString(allocator, res));
  }
  case vm::PCallCode::Sleep:
    std::this_thread::sleep_for(std::chrono::milliseconds(evalStack.peek().getInt()));
    return ResultCode::Ok;
  case vm::PCallCode::Assert: {
    auto* msg = getStringRef(evalStack.pop());
    auto cond = evalStack.peek().getInt();
    if (cond == 0) {
      iface.conWrite("Assertion failed: ", 18);
      iface.conWrite(msg->getDataPtr(), msg->getSize());
      iface.conWrite(&newl, 1);
      return ResultCode::AssertFailed;
    }
    return ResultCode::Ok;
  }
  }
  return ResultCode::InvalidAssembly;
}

} // namespace vm::internal
