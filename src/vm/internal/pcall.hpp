#pragma once
#include "internal/stack.hpp"
#include "internal/string_utilities.hpp"
#include "vm/pcall_code.hpp"
#include "vm/result_code.hpp"
#include <chrono>
#include <thread>

namespace vm::internal {

const auto static newl = '\n';

template <unsigned int StackSize, typename PlatformInterface>
auto inline pcall(
    Stack<StackSize>& stack,
    Allocator* allocator,
    PlatformInterface& iface,
    PCallCode code,
    ResultCode* resultCode) noexcept -> void {

#define PUSH(VAL)                                                                                  \
  if (!stack.push(VAL)) {                                                                          \
    *resultCode = ResultCode::StackOverflow;                                                       \
  }
#define PUSH_REF(VAL) PUSH(refValue(VAL))
#define PUSH_INT(VAL) PUSH(intValue(VAL))
#define POP() stack.pop()
#define POP_INT() POP().getInt()
#define PEEK() stack.peek()
#define PEEK_INT() PEEK().getInt()

  switch (code) {
  case vm::PCallCode::ConWriteChar: {
    auto c = static_cast<char>(PEEK_INT());
    iface.conWrite(&c, 1);
  } break;
  case vm::PCallCode::ConWriteString: {
    auto* strRef = getStringRef(PEEK());
    iface.conWrite(strRef->getDataPtr(), strRef->getSize());
  } break;
  case vm::PCallCode::ConWriteStringLine: {
    auto* strRef = getStringRef(PEEK());
    iface.conWrite(strRef->getDataPtr(), strRef->getSize());
    iface.conWrite(&newl, 1);
  } break;
  case vm::PCallCode::ConReadChar: {
    PUSH_INT(iface.conRead());
  } break;
  case vm::PCallCode::ConReadStringLine: {
    std::string line = {};
    while (true) {
      const auto c = iface.conRead();
      if (c == '\0' || c == '\n') {
        break;
      }
      line += c;
    }
    PUSH_REF(toString(allocator, line));
  } break;
  case vm::PCallCode::GetEnvArg: {
    auto* res = iface.getEnvArg(PEEK_INT());
    if (res == nullptr) {
      PUSH_REF(allocator->allocStr(0).first);
    } else {
      PUSH_REF(toString(allocator, res));
    }
  } break;
  case vm::PCallCode::GetEnvArgCount: {
    PUSH_INT(iface.getEnvArgCount());
  } break;
  case vm::PCallCode::GetEnvVar: {
    auto* nameStrRef = getStringRef(stack.pop());
    auto* res        = iface.getEnvVar(nameStrRef->getDataPtr());
    if (res == nullptr) {
      PUSH_REF(allocator->allocStr(0).first);
    } else {
      PUSH_REF(toString(allocator, res));
    }
  } break;
  case vm::PCallCode::Sleep: {
    std::this_thread::sleep_for(std::chrono::milliseconds(PEEK_INT()));
  } break;
  case vm::PCallCode::Assert: {
    auto* msg = getStringRef(POP());
    auto cond = PEEK_INT();
    if (cond == 0) {
      iface.conWrite("Assertion failed: ", 18);
      iface.conWrite(msg->getDataPtr(), msg->getSize());
      iface.conWrite(&newl, 1);
      *resultCode = ResultCode::AssertFailed;
    }
  } break;
  default:
    *resultCode = ResultCode::InvalidAssembly;
  }

#undef PUSH
#undef PUSH_REF
#undef PUSH_INT
#undef POP
#undef POP_INT
#undef PEEK
#undef PEEK_INT
}

} // namespace vm::internal
