#include "internal/executor.hpp"
#include "internal/allocator.hpp"
#include "internal/pcall.hpp"
#include "internal/ref_future.hpp"
#include "internal/ref_long.hpp"
#include "internal/ref_string.hpp"
#include "internal/ref_struct.hpp"
#include "internal/stack.hpp"
#include "internal/string_utilities.hpp"
#include "novasm/op_code.hpp"
#include "novasm/pcall_code.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/platform/terminal_interface.hpp"
#include <cmath>
#include <immintrin.h>
#include <thread>

namespace vm::internal {

template <typename Type>
inline auto readAsm(const uint8_t** ip) {
  // TODO(bastian): Handle endianess differences.
  const Type v = *reinterpret_cast<const Type*>(*ip); // NOLINT: Reinterpret cast
  *ip += sizeof(Type);
  return v;
}

inline auto call(
    const novasm::Assembly* assembly,
    BasicStack* stack,
    ExecutorHandle* execHandle,
    const uint8_t** ip,
    Value** sh,
    uint8_t argCount,
    uint32_t tgtIpOffset) -> bool {

  /* Arguments are pushed on the stack before the call instruction, we shift over the arguments
  to make space for the return instruction, and the return stack home ptr. */

  const int sfMetaSize = 2; // Return ip and return stack-home.

  auto* newSh = stack->getNext() - argCount + sfMetaSize;

  // Allocate space on the stack for the stackframe meta-data.
  if (!stack->alloc(2)) {
    execHandle->setState(ExecState::StackOverflow);
    return false;
  }

  // Move the arguments to the beginning of the stack-home for the new stack frame.
  std::memcpy(newSh, newSh - sfMetaSize, sizeof(Value) * argCount);

  // Save the return instruction pointer and stack-home.
  *(newSh - 2) = uintValue(assembly->getOffset(*ip));
  *(newSh - 1) = rawPtrValue(*sh);

  // Setup the ip and stack-home for the new stack frame.
  *ip = assembly->getIp(tgtIpOffset);
  *sh = newSh;
  return true;
}

inline auto callTail(
    const novasm::Assembly* assembly,
    BasicStack* stack,
    const uint8_t** ip,
    Value* sh,
    uint8_t argCount,
    uint32_t tgtIpOffset) -> void {

  /* In case of a tail-call we discard our current stack-frame, we copy the arguments to the
  beginning of the current-stack frame and update the ip. */

  // Move the arguments to the beginning of the current stack home.
  std::memcpy(sh, stack->getNext() - argCount, sizeof(Value) * argCount);

  stack->rewindToNext(sh + argCount); // Discard any extra values on the stack.
  *ip = assembly->getIp(tgtIpOffset);
}

inline auto pushClosure(
    BasicStack* stack,
    ExecutorHandle* execHandle,
    const Value& closureVal,
    uint8_t* boundArgCount,
    uint32_t* ipOffset) -> bool {

  auto* closureStruct = getStructRef(closureVal);
  *boundArgCount      = closureStruct->getFieldCount() - 1U;
  assert(closureStruct->getFieldCount() > 0);

  // Push all bound arguments on the stack.
  for (auto i = 0U; i != *boundArgCount; ++i) {
    if (!stack->push(closureStruct->getField(i))) {
      execHandle->setState(ExecState::StackOverflow);
      return false;
    }
  }

  *ipOffset = closureStruct->getField(*boundArgCount).getUInt();
  return true;
}

template <typename PlatformInterface>
inline auto fork(
    const novasm::Assembly* assembly,
    PlatformInterface* iface,
    ExecutorRegistry* execRegistry,
    Allocator* allocator,
    BasicStack* stack,
    ExecutorHandle* execHandle,
    uint8_t argCount,
    uint32_t entryIpOffset) -> bool {

  // Create a future object to interact with the fork.
  auto* future = allocator->allocPlain<FutureRef>();
  if (future == nullptr) {
    execHandle->setState(ExecState::AllocFailed);
    return false;
  }

  auto* argSource  = stack->getNext() - argCount;
  auto* threadFunc = &execute<PlatformInterface>;
  std::thread(
      threadFunc,
      assembly,
      iface,
      execRegistry,
      allocator,
      entryIpOffset,
      argCount,
      argSource,
      future)
      .detach();

  // Wait until the fork executor has started.
  std::this_thread::yield();
  while (!future->getStarted().load(std::memory_order_acquire)) {
    _mm_pause();
  }

  /* Note its important to leave the arguments alone until the fork executor has started, as until
  then the executor might still be reading them. */

  // Remove the arguments from the stack.
  stack->rewindToNext(stack->getNext() - argCount);

  // Push the future on the stack.
  if (!stack->push(refValue(future))) {
    execHandle->setState(ExecState::StackOverflow);
    return false;
  }
  return true;
}

template <typename PlatformInterface>
auto execute(
    const novasm::Assembly* assembly,
    PlatformInterface* iface,
    ExecutorRegistry* execRegistry,
    Allocator* allocator,
    uint32_t entryIpOffset,
    uint8_t entryArgCount,
    Value* entryArgSource,
    FutureRef* promise) noexcept -> ExecState {

  using OpCode    = novasm::OpCode;
  using PCallCode = novasm::PCallCode;

#define READ_BYTE() readAsm<uint8_t>(&ip)
#define READ_INT() readAsm<int32_t>(&ip)
#define READ_UINT() readAsm<uint32_t>(&ip)
#define READ_LONG() readAsm<int64_t>(&ip)
#define READ_FLOAT() readAsm<float>(&ip)
#define SALLOC(COUNT)                                                                              \
  if (!stack.alloc(COUNT)) {                                                                       \
    execHandle.setState(ExecState::StackOverflow);                                                 \
    goto End;                                                                                      \
  }
#define PUSH(VAL)                                                                                  \
  if (!stack.push(VAL)) {                                                                          \
    execHandle.setState(ExecState::StackOverflow);                                                 \
    goto End;                                                                                      \
  }
#define PUSH_UINT(VAL) PUSH(uintValue(VAL))
#define PUSH_INT(VAL) PUSH(intValue(VAL))
#define PUSH_LONG(VAL)                                                                             \
  {                                                                                                \
    int64_t v = VAL;                                                                               \
    if (v > 0) {                                                                                   \
      PUSH(posLongValue(v));                                                                       \
    } else {                                                                                       \
      PUSH_REF(allocator->allocPlain<LongRef>(v));                                                 \
    }                                                                                              \
  }
#define PUSH_BOOL(VAL) PUSH(intValue(VAL))
#define PUSH_FLOAT(VAL) PUSH(floatValue(VAL))
#define PUSH_REF(VAL)                                                                              \
  {                                                                                                \
    auto* refPtr = VAL;                                                                            \
    if (refPtr == nullptr) {                                                                       \
      execHandle.setState(ExecState::AllocFailed);                                                 \
      goto End;                                                                                    \
    }                                                                                              \
    PUSH(refValue(refPtr));                                                                        \
  }
#define PUSH_CLOSURE(VAL, RES_BOUND_ARG_COUNT, RES_TGT_IP_OFFSET)                                  \
  if (!pushClosure(&stack, &execHandle, VAL, RES_BOUND_ARG_COUNT, RES_TGT_IP_OFFSET)) {            \
    goto End;                                                                                      \
  }
#define PEEK() stack.peek()
#define POP() stack.pop()
#define POP_UINT() POP().getUInt()
#define POP_INT() POP().getInt()
#define POP_FLOAT() POP().getFloat()
#define CALL(ARG_COUNT, TGT_IP)                                                                    \
  if (!call(assembly, &stack, &execHandle, &ip, &sh, ARG_COUNT, TGT_IP)) {                         \
    goto End;                                                                                      \
  }
#define CALL_TAIL(ARG_COUNT, TGT_IP) callTail(assembly, &stack, &ip, sh, ARG_COUNT, TGT_IP)
#define CALL_FORKED(ARG_COUNT, TGT_IP)                                                             \
  if (!fork(assembly, iface, execRegistry, allocator, &stack, &execHandle, ARG_COUNT, TGT_IP)) {   \
    goto End;                                                                                      \
  }

  // Setup state.
  auto stack      = BasicStack{};
  auto execHandle = ExecutorHandle{&stack};
  execRegistry->registerExecutor(&execHandle);

  // If we are given a promise to fill then push it on the stack, its important to be on the stack
  // so the garbage collector can 'see' it. We place the promise one position before the root
  // stack-home to make it invisible to the running assembly.
  if (promise) {
    stack.push(refValue(promise));
  }
  const uint8_t* ip = assembly->getIp(entryIpOffset); // Current instruction-pointer.
  Value* sh     = stack.getNext(); // Current 'home' for this stack-frame, used to store variables.
  Value* rootSh = sh;

  // Push the entry args on the stack (if any), these are available at the root stack-home.
  if (entryArgSource && entryArgCount > 0) {
    SALLOC(entryArgCount);
    std::memcpy(sh, entryArgSource, sizeof(Value) * entryArgCount);
  }

  if (promise) {
    // Signal that we've started running the promise, this also tells to outside that we are done
    // with accessing the entry arguments and have properly placed the future on our stack.
    promise->getStarted().store(true, std::memory_order_release);
  }

  // Start executing instructions.
  while (true) {
    switch (readAsm<OpCode>(&ip)) {
    case OpCode::LoadLitInt: {
      PUSH_INT(READ_INT());
    } break;
    case OpCode::LoadLitIntSmall: {
      PUSH_INT(READ_BYTE());
    } break;
    case OpCode::LoadLitInt0: {
      PUSH_INT(0);
    } break;
    case OpCode::LoadLitInt1: {
      PUSH_INT(1);
    } break;
    case OpCode::LoadLitLong: {
      PUSH_LONG(READ_LONG());
    } break;
    case OpCode::LoadLitFloat: {
      PUSH_FLOAT(READ_FLOAT());
    } break;
    case OpCode::LoadLitString: {
      const auto& litStr = assembly->getLitString(READ_UINT());
      PUSH_REF(allocator->allocStrLit(litStr));
    } break;
    case OpCode::LoadLitIp: {
      PUSH_UINT(READ_UINT());
    } break;

    case OpCode::StackAlloc: {
      const auto amount = READ_BYTE();
      assert(amount > 0);
      SALLOC(amount);
    } break;
    case OpCode::StackStore: {
      *(sh + READ_BYTE()) = stack.pop();
    } break;
    case OpCode::StackLoad: {
      PUSH(*(sh + READ_BYTE()));
    } break;

    case OpCode::AddInt: {
      PUSH_INT(POP_INT() + POP_INT());
    } break;
    case OpCode::AddLong: {
      const auto val = getLong(POP()) + getLong(POP());
      PUSH_LONG(val);
    } break;
    case OpCode::AddFloat: {
      PUSH_FLOAT(POP_FLOAT() + POP_FLOAT());
    } break;
    case OpCode::AddString: {
      auto* b = getStringRef(POP());
      auto* a = getStringRef(POP());
      PUSH_REF(concatString(allocator, a, b));
    } break;
    case OpCode::AddChar: {
      auto b = static_cast<uint8_t>(POP_INT());
      auto a = static_cast<uint8_t>(POP_INT());
      PUSH_REF(charsToString(allocator, a, b));
    } break;
    case OpCode::SubInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a - b);
    } break;
    case OpCode::SubLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      PUSH_LONG(a - b);
    } break;
    case OpCode::SubFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(a - b);
    } break;
    case OpCode::MulInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a * b);
    } break;
    case OpCode::MulLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      PUSH_LONG(a * b);
    } break;
    case OpCode::MulFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(a * b);
    } break;
    case OpCode::DivInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      if (b == 0) {
        execHandle.setState(ExecState::DivByZero);
        goto End;
      }
      PUSH_INT(a / b);
    } break;
    case OpCode::DivLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      if (b == 0) {
        execHandle.setState(ExecState::DivByZero);
        goto End;
      }
      PUSH_LONG(a / b);
    } break;
    case OpCode::DivFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(a / b);
    } break;
    case OpCode::RemInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      if (b == 0) {
        execHandle.setState(ExecState::DivByZero);
        goto End;
      }
      PUSH_INT(a % b);
    } break;
    case OpCode::RemLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      if (b == 0) {
        execHandle.setState(ExecState::DivByZero);
        goto End;
      }
      PUSH_LONG(a % b);
    } break;
    case OpCode::ModFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(fmodf(a, b));
    } break;
    case OpCode::PowFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(powf(a, b));
    } break;
    case OpCode::SqrtFloat: {
      PUSH_FLOAT(sqrtf(POP_FLOAT()));
    } break;
    case OpCode::SinFloat: {
      PUSH_FLOAT(sinf(POP_FLOAT()));
    } break;
    case OpCode::CosFloat: {
      PUSH_FLOAT(cosf(POP_FLOAT()));
    } break;
    case OpCode::TanFloat: {
      PUSH_FLOAT(tanf(POP_FLOAT()));
    } break;
    case OpCode::ASinFloat: {
      PUSH_FLOAT(asinf(POP_FLOAT()));
    } break;
    case OpCode::ACosFloat: {
      PUSH_FLOAT(acosf(POP_FLOAT()));
    } break;
    case OpCode::ATanFloat: {
      PUSH_FLOAT(atanf(POP_FLOAT()));
    } break;
    case OpCode::ATan2Float: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(atan2f(a, b));
    } break;
    case OpCode::NegInt: {
      PUSH_INT(-POP_INT());
    } break;
    case OpCode::NegLong: {
      PUSH_LONG(-getLong(POP()));
    } break;
    case OpCode::NegFloat: {
      PUSH_FLOAT(-POP_FLOAT());
    } break;
    case OpCode::LogicInvInt: {
      PUSH_BOOL(POP_INT() == 0);
    } break;
    case OpCode::ShiftLeftInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a << b); // NOLINT: Signed bitwise operand
    } break;
    case OpCode::ShiftRightInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a >> b); // NOLINT: Signed bitwise operand
    } break;
    case OpCode::AndInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a & b); // NOLINT: Signed bitwise operand
    } break;
    case OpCode::OrInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a | b); // NOLINT: Signed bitwise operand
    } break;
    case OpCode::XorInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a ^ b); // NOLINT: Signed bitwise operand
    } break;
    case OpCode::InvInt: {
      PUSH_INT(~POP_INT()); // NOLINT: Signed bitwise operand
    } break;
    case OpCode::LengthString: {
      PUSH_INT(getStringRef(POP())->getSize());
    } break;
    case OpCode::IndexString: {
      auto index = POP_INT();
      auto* str  = getStringRef(POP());
      PUSH_INT(indexString(str, index));
    } break;
    case OpCode::SliceString: {
      auto end   = POP_INT();
      auto start = POP_INT();
      auto* str  = getStringRef(POP());
      PUSH_REF(sliceString(allocator, str, start, end));
    } break;

    case OpCode::CheckEqInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_BOOL(a == b);
    } break;
    case OpCode::CheckEqLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      PUSH_BOOL(a == b);
    } break;
    case OpCode::CheckEqFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_BOOL(a == b);
    } break;
    case OpCode::CheckEqString: {
      auto* b = getStringRef(POP());
      auto* a = getStringRef(POP());
      PUSH_BOOL(checkStringEq(a, b));
    } break;
    case OpCode::CheckEqIp: {
      auto b = POP_UINT();
      auto a = POP_UINT();
      PUSH_BOOL(a == b);
    } break;
    case OpCode::CheckEqCallDynTgt: {
      // Compare the target instruction pointers (which for closure structs are stored in the last
      // field). Note: This does not compare bound arguments in a closure struct, main reason is
      // that we have no type information for those.
      auto b   = POP();
      auto bIp = (b.isRef() ? getStructRef(b)->getLastField() : b).getUInt();
      auto a   = POP();
      auto aIp = (a.isRef() ? getStructRef(a)->getLastField() : a).getUInt();
      PUSH_BOOL(aIp == bIp);
    } break;
    case OpCode::CheckGtInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_BOOL(a > b);
    } break;
    case OpCode::CheckGtLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      PUSH_BOOL(a > b);
    } break;
    case OpCode::CheckGtFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_BOOL(a > b);
    } break;
    case OpCode::CheckLeInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_BOOL(a < b);
    } break;
    case OpCode::CheckLeLong: {
      auto b = getLong(POP());
      auto a = getLong(POP());
      PUSH_BOOL(a < b);
    } break;
    case OpCode::CheckLeFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_BOOL(a < b);
    } break;

    case OpCode::ConvIntLong: {
      PUSH_LONG(static_cast<int64_t>(POP_INT()));
    } break;
    case OpCode::ConvIntFloat: {
      PUSH_FLOAT(static_cast<float>(POP_INT()));
    } break;
    case OpCode::ConvLongInt: {
      PUSH_INT(static_cast<int32_t>(getLong(POP())));
    } break;
    case OpCode::ConvFloatInt: {
      PUSH_INT(static_cast<int32_t>(POP_FLOAT()));
    } break;
    case OpCode::ConvIntString: {
      PUSH_REF(intToString(allocator, POP_INT()));
    } break;
    case OpCode::ConvLongString: {
      PUSH_REF(intToString(allocator, getLong(POP())));
    } break;
    case OpCode::ConvFloatString: {
      PUSH_REF(floatToString(allocator, POP_FLOAT()));
    } break;
    case OpCode::ConvCharString: {
      PUSH_REF(charToString(allocator, static_cast<uint8_t>(POP_INT())));
    } break;
    case OpCode::ConvIntChar: {
      PUSH_INT(static_cast<uint8_t>(POP_INT()));
    } break;

    case OpCode::MakeStruct: {
      const auto fieldCount = READ_BYTE();
      assert(fieldCount > 0);

      auto structRefAlloc = allocator->allocStruct(fieldCount);
      if (structRefAlloc.first == nullptr) {
        execHandle.setState(ExecState::AllocFailed);
        goto End;
      }

      // Important to iterate in reverse, as the fields are in reverse order on the stack.
      for (auto fieldIndex = fieldCount; fieldIndex-- > 0;) {
        *(structRefAlloc.second + fieldIndex) = POP();
      }
      PUSH_REF(structRefAlloc.first);
    } break;
    case OpCode::LoadStructField: {
      const auto fieldIndex = READ_BYTE();
      auto* s               = getStructRef(POP());
      PUSH(s->getField(fieldIndex));
    } break;

    case OpCode::Jump: {
      ip = assembly->getIp(READ_UINT());
    } break;
    case OpCode::JumpIf: {
      auto ipOffset = READ_UINT();
      if (POP_INT() != 0) {
        ip = assembly->getIp(ipOffset);
      }
    } break;

    case OpCode::Call: {
      const auto argCount    = READ_BYTE();
      const auto tgtIpOffset = READ_UINT();
      CALL(argCount, tgtIpOffset);
    } break;
    case OpCode::CallTail: {
      // Place a trap here as with tail-calls is possible to have code that runs for a long time
      // without ever hitting a 'ret' instruction.
      if (execHandle.trap()) {
        goto End;
      }

      const auto argCount    = READ_BYTE();
      const auto tgtIpOffset = READ_UINT();
      CALL_TAIL(argCount, tgtIpOffset);
    } break;
    case OpCode::CallForked: {
      const auto argCount    = READ_BYTE();
      const auto tgtIpOffset = READ_UINT();
      CALL_FORKED(argCount, tgtIpOffset);
    } break;
    case OpCode::CallDyn: {
      const auto argCount = READ_BYTE();
      auto tgt            = POP();
      if (tgt.isRef()) { // Target is a closure containing bound args and a instruction pointer.
        uint8_t boundArgCount;
        uint32_t tgtIpOffset;
        PUSH_CLOSURE(tgt, &boundArgCount, &tgtIpOffset);
        CALL(argCount + boundArgCount, tgtIpOffset);
      } else { // Target is a instruction pointer only.
        CALL(argCount, tgt.getUInt());
      }
    } break;
    case OpCode::CallDynTail: {
      // Place a trap here as with tail-calls is possible to have code that runs for a long time
      // without ever hitting a 'ret' instruction.
      if (execHandle.trap()) {
        goto End;
      }

      const auto argCount = READ_BYTE();
      auto tgt            = POP();
      if (tgt.isRef()) { // Target is a closure containing bound args and a instruction pointer.
        uint8_t boundArgCount;
        uint32_t tgtIpOffset;
        PUSH_CLOSURE(tgt, &boundArgCount, &tgtIpOffset);
        CALL_TAIL(argCount + boundArgCount, tgtIpOffset);
      } else { // Target is a instruction pointer only.
        CALL_TAIL(argCount, tgt.getUInt());
      }
    } break;
    case OpCode::CallDynForked: {
      const auto argCount = READ_BYTE();
      auto tgt            = POP();
      if (tgt.isRef()) { // Target is a closure containing bound args and a instruction pointer.
        uint8_t boundArgCount;
        uint32_t tgtIpOffset;
        PUSH_CLOSURE(tgt, &boundArgCount, &tgtIpOffset);
        CALL_FORKED(argCount + boundArgCount, tgtIpOffset);
      } else { // Target is a instruction pointer only.
        CALL_FORKED(argCount, tgt.getUInt());
      }
    } break;
    case OpCode::PCall: {
      pcall(iface, allocator, &stack, &execHandle, readAsm<PCallCode>(&ip));
      if (execHandle.getState(std::memory_order_relaxed) != ExecState::Running) {
        assert(execHandle.getState(std::memory_order_relaxed) != ExecState::Success);
        goto End;
      }
    } break;
    case OpCode::Ret: {
      if (execHandle.trap()) {
        goto End;
      }

      // Check if this returns from the root-stack frame.
      if (sh == rootSh) {
        execHandle.setState(ExecState::Success);
        goto End;
      }
      assert(stack.getSize() >= 3); // Should at least contain a return ip and sh and ret value.

      auto retVal = POP();

      // Rewind this entire stack-frame (+ 2 for the stack-frame meta-data).
      stack.rewindToNext(sh - 2);

      // Note this assumes that the rewinding does not actually invalidate the memory (which it
      // doesn't).
      ip = assembly->getIp((sh - 2)->getUInt());
      sh = (sh - 1)->getRawPtr<Value>();

      // Place the return-value on the stack.
      PUSH(retVal);
    } break;

    case OpCode::FutureWaitNano: {
      int64_t timeout = getLong(POP());
      if (timeout <= 0) {
        auto* future = getFutureRef(POP());
        PUSH_BOOL(future->poll() != ExecState::Running);
        break;
      }

      // Get the future but leave it on the stack, reason is gc could run while we are blocked.
      auto* future = getFutureRef(PEEK());

      execHandle.setState(ExecState::Paused);
      auto success = future->waitNano(timeout);
      execHandle.setState(ExecState::Running);

      if (execHandle.trap()) {
        goto End;
      }

      POP(); // Pop the future itself from the stack.
      PUSH_BOOL(success);
    } break;
    case OpCode::FutureBlock: {
      // The the future but leave it on the stack, reason is gc could run while we are blocked.
      auto* future = getFutureRef(PEEK());

      execHandle.setState(ExecState::Paused);
      auto futureState = future->block();
      execHandle.setState(ExecState::Running);

      if (execHandle.trap()) {
        goto End;
      }

      assert(futureState != ExecState::Running);
      if (futureState == ExecState::Success) {
        POP(); // Pop the future itself from the stack.
        PUSH(future->getResult());
      } else {
        // If the future failed then we fail our executor also.
        execHandle.setState(futureState);
        goto End;
      }
    } break;
    case OpCode::Dup:
      PUSH(PEEK());
      break;
    case OpCode::Pop:
      POP();
      break;

    case OpCode::Fail:
    default:
      execHandle.setState(ExecState::InvalidAssembly);
      goto End;
    }
  }

End:
  // If we are backing a promise then fill-in the results and notify all waiters.
  auto endState = execHandle.getState(std::memory_order_relaxed);
  if (promise) {
    if (endState == ExecState::Success) {
      promise->setResult(POP());
    }
    {
      auto lk = std::lock_guard<std::mutex>{promise->getMutex()};
      promise->setState(endState);
    }
    promise->getCondVar().notify_all();
  }
  execRegistry->unregisterExecutor(&execHandle);
  return endState;

#undef READ_BYTE
#undef READ_INT
#undef READ_UINT
#undef READ_LONG
#undef READ_FLOAT
#undef SALLOC
#undef PUSH
#undef PUSH_UINT
#undef PUSH_INT
#undef PUSH_LONG
#undef PUSH_BOOL
#undef PUSH_FLOAT
#undef PUSH_REF
#undef PUSH_CLOSURE
#undef PEEK
#undef POP
#undef POP_UINT
#undef POP_INT
#undef POP_FLOAT
#undef CALL
#undef CALL_TAIL
#undef CALL_FORKED
}

// Explicit instantiations.
template ExecState execute(
    const novasm::Assembly* assembly,
    platform::MemoryInterface* iface,
    ExecutorRegistry* execRegistry,
    Allocator* allocator,
    uint32_t entryIpOffset,
    uint8_t entryArgCount,
    Value* entryArgSource,
    FutureRef* promise) noexcept;

template ExecState execute(
    const novasm::Assembly* assembly,
    platform::TerminalInterface* iface,
    ExecutorRegistry* execRegistry,
    Allocator* allocator,
    uint32_t entryIpOffset,
    uint8_t entryArgCount,
    Value* entryArgSource,
    FutureRef* promise) noexcept;

} // namespace vm::internal
