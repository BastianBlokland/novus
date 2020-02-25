#pragma once
#include "internal/executor_handle.hpp"
#include "internal/stack.hpp"
#include "internal/string_utilities.hpp"
#include "novasm/pcall_code.hpp"
#include "vm/exec_state.hpp"
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
    novasm::PCallCode code) noexcept -> void {
  assert(iface && allocator && stack && execHandle);

  using PCallCode = novasm::PCallCode;

#define PUSH(VAL)                                                                                  \
  if (!stack->push(VAL)) {                                                                         \
    execHandle->setState(ExecState::StackOverflow);                                                \
    return;                                                                                        \
  }
#define PUSH_INT(VAL) PUSH(intValue(VAL))
#define PUSH_LONG(VAL)                                                                             \
  {                                                                                                \
    int64_t v = VAL;                                                                               \
    if (v > 0) {                                                                                   \
      PUSH(posLongValue(v));                                                                       \
    } else {                                                                                       \
      PUSH_REF(allocator->allocLong(v));                                                           \
    }                                                                                              \
  }
#define PUSH_REF(VAL)                                                                              \
  {                                                                                                \
    auto* refPtr = VAL;                                                                            \
    if (refPtr == nullptr) {                                                                       \
      execHandle->setState(ExecState::AllocFailed);                                                \
      return;                                                                                      \
    }                                                                                              \
    PUSH(refValue(refPtr));                                                                        \
  }
#define POP() stack->pop()
#define POP_INT() POP().getInt()
#define PEEK() stack->peek()
#define PEEK_INT() PEEK().getInt()

  switch (code) {
  case PCallCode::ConWriteChar: {
    auto c = static_cast<char>(PEEK_INT());
    iface->lockConWrite();
    iface->conWrite(&c, 1);
    iface->unlockConWrite();
  } break;
  case PCallCode::ConWriteString: {
    auto* strRef = getStringRef(PEEK());
    iface->lockConWrite();
    iface->conWrite(strRef->getDataPtr(), strRef->getSize());
    iface->unlockConWrite();
  } break;
  case PCallCode::ConWriteStringLine: {
    auto* strRef = getStringRef(PEEK());
    iface->lockConWrite();
    iface->conWrite(strRef->getDataPtr(), strRef->getSize());
    iface->conWrite(&newl, 1);
    iface->unlockConWrite();
  } break;

  case PCallCode::ConReadChar: {
    execHandle->setState(ExecState::Paused);
    iface->lockConRead();
    auto readChar = iface->conRead();
    iface->unlockConRead();
    execHandle->setState(ExecState::Running);
    execHandle->trap();
    PUSH_INT(readChar);
  } break;
  case PCallCode::ConReadStringLine: {
    execHandle->setState(ExecState::Paused);
    iface->lockConRead();
    std::string line = {};
    while (true) {
      const auto c = iface->conRead();
      if (c == '\0' || c == '\n') {
        break;
      }
      line += c;
    }
    iface->unlockConRead();
    execHandle->setState(ExecState::Running);
    execHandle->trap();
    PUSH_REF(toStringRef(allocator, line));
  } break;

  case PCallCode::GetEnvArg: {
    auto* res = iface->getEnvArg(POP_INT());
    PUSH_REF(res == nullptr ? allocator->allocStr(0).first : toStringRef(allocator, res));
  } break;
  case PCallCode::GetEnvArgCount: {
    PUSH_INT(iface->getEnvArgCount());
  } break;
  case PCallCode::GetEnvVar: {
    auto* nameStrRef = getStringRef(POP());
    auto* res        = iface->getEnvVar(nameStrRef->getDataPtr());
    PUSH_REF(res == nullptr ? allocator->allocStr(0).first : toStringRef(allocator, res));
  } break;

  case PCallCode::ClockMicroSinceEpoch: {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    PUSH_LONG(std::chrono::duration_cast<std::chrono::microseconds>(now).count());
  } break;
  case PCallCode::ClockNanoSteady: {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    PUSH_LONG(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
  } break;

  case PCallCode::SleepNano: {
    execHandle->setState(ExecState::Paused);
    std::this_thread::sleep_for(std::chrono::nanoseconds(getLong(PEEK())));
    execHandle->setState(ExecState::Running);
    execHandle->trap();
  } break;
  case PCallCode::Assert: {
    auto* msg = getStringRef(POP());
    auto cond = PEEK_INT();
    if (cond == 0) {
      iface->lockConWrite();
      iface->conWrite("Assertion failed: ", 18);
      iface->conWrite(msg->getDataPtr(), msg->getSize());
      iface->conWrite(&newl, 1);
      iface->unlockConWrite();
      execHandle->setState(ExecState::AssertFailed);
    }
  } break;
  default:
    execHandle->setState(ExecState::InvalidAssembly);
  }

#undef PUSH
#undef PUSH_INT
#undef PUSH_LONG
#undef PUSH_REF
#undef POP
#undef POP_INT
#undef PEEK
#undef PEEK_INT
}

} // namespace vm::internal
