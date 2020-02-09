#pragma once
#include "internal/executor_handle.hpp"
#include "internal/stack.hpp"
#include "internal/string_utilities.hpp"
#include "vm/exec_state.hpp"
#include "vm/pcall_code.hpp"
#include <chrono>
#include <thread>

namespace vm::internal {

const auto static newl = '\n';

template <typename PlatformInterface>
auto inline pcall(
    PlatformInterface* iface,
    Allocator* allocator,
    BasicStack* stack,
    ExecutorHandle* execHandle,
    PCallCode code) noexcept -> void {
  assert(iface && allocator && stack && execHandle);

#define PUSH(VAL)                                                                                  \
  if (!stack->push(VAL)) {                                                                         \
    execHandle->setState(ExecState::StackOverflow);                                                \
  }
#define PUSH_REF(VAL) PUSH(refValue(VAL))
#define PUSH_INT(VAL) PUSH(intValue(VAL))
#define POP() stack->pop()
#define POP_INT() POP().getInt()
#define PEEK() stack->peek()
#define PEEK_INT() PEEK().getInt()

  switch (code) {
  case vm::PCallCode::ConWriteChar: {
    auto c = static_cast<char>(PEEK_INT());
    iface->lockConWrite();
    iface->conWrite(&c, 1);
    iface->unlockConWrite();
  } break;
  case vm::PCallCode::ConWriteString: {
    auto* strRef = getStringRef(PEEK());
    iface->lockConWrite();
    iface->conWrite(strRef->getDataPtr(), strRef->getSize());
    iface->unlockConWrite();
  } break;
  case vm::PCallCode::ConWriteStringLine: {
    auto* strRef = getStringRef(PEEK());
    iface->lockConWrite();
    iface->conWrite(strRef->getDataPtr(), strRef->getSize());
    iface->conWrite(&newl, 1);
    iface->unlockConWrite();
  } break;
  case vm::PCallCode::ConReadChar: {
    execHandle->setState(ExecState::Paused);
    PUSH_INT(iface->conRead());
    execHandle->setState(ExecState::Running);
    execHandle->trap();
  } break;
  case vm::PCallCode::ConReadStringLine: {
    execHandle->setState(ExecState::Paused);
    std::string line = {};
    while (true) {
      const auto c = iface->conRead();
      if (c == '\0' || c == '\n') {
        break;
      }
      line += c;
    }
    execHandle->setState(ExecState::Running);
    execHandle->trap();
    PUSH_REF(toString(allocator, line));
  } break;
  case vm::PCallCode::GetEnvArg: {
    auto* res = iface->getEnvArg(PEEK_INT());
    if (res == nullptr) {
      PUSH_REF(allocator->allocStr(0).first);
    } else {
      PUSH_REF(toString(allocator, res));
    }
  } break;
  case vm::PCallCode::GetEnvArgCount: {
    PUSH_INT(iface->getEnvArgCount());
  } break;
  case vm::PCallCode::GetEnvVar: {
    auto* nameStrRef = getStringRef(POP());
    auto* res        = iface->getEnvVar(nameStrRef->getDataPtr());
    if (res == nullptr) {
      PUSH_REF(allocator->allocStr(0).first);
    } else {
      PUSH_REF(toString(allocator, res));
    }
  } break;
  case vm::PCallCode::Sleep: {
    execHandle->setState(ExecState::Paused);
    std::this_thread::sleep_for(std::chrono::milliseconds(PEEK_INT()));
    execHandle->setState(ExecState::Running);
    execHandle->trap();
  } break;
  case vm::PCallCode::Assert: {
    auto* msg = getStringRef(POP());
    auto cond = PEEK_INT();
    if (cond == 0) {
      iface->conWrite("Assertion failed: ", 18);
      iface->conWrite(msg->getDataPtr(), msg->getSize());
      iface->conWrite(&newl, 1);
      execHandle->setState(ExecState::AssertFailed);
    }
  } break;
  default:
    execHandle->setState(ExecState::InvalidAssembly);
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
