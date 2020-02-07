#include "internal/executor.hpp"
#include "internal/allocator.hpp"
#include "internal/pcall.hpp"
#include "internal/ref_string.hpp"
#include "internal/stack.hpp"
#include "internal/string_utilities.hpp"
#include "vm/exec_state.hpp"
#include "vm/op_code.hpp"
#include "vm/pcall_code.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/platform/terminal_interface.hpp"
#include <cmath>

namespace vm::internal {

static const int StackSize = 10240;

template <typename Type>
inline auto readAsm(const uint8_t** ip) {
  // TODO(bastian): Handle endianess differences.
  const Type v = *reinterpret_cast<const Type*>(*ip); // NOLINT: Reinterpret cast
  *ip += sizeof(Type);
  return v;
}

template <unsigned int StackSize>
inline auto call(
    Stack<StackSize>& stack,
    const Assembly& assembly,
    const uint8_t** ip,
    Value** sh,
    uint8_t argCount,
    uint32_t tgtIpOffset) -> bool {

  /* Arguments are pushed on the stack before the call instruction, we shift over the arguments
  to make space for the return instruction, and the return stack home ptr. */

  const int sfMetaSize = 2; // Return ip and return stack-home.

  auto* newSh = stack.getNext() - argCount + sfMetaSize;

  // Allocate space on the stack for the stackframe meta-data.
  if (!stack.alloc(2)) {
    return false;
  }

  // Move the arguments to the beginning of the stack-home for the new stack frame.
  std::memcpy(newSh, newSh - sfMetaSize, sizeof(Value) * argCount);

  // Save the return instruction pointer and stack-home.
  *(newSh - 2) = uintValue(assembly.getOffset(*ip));
  *(newSh - 1) = rawPtrValue(*sh);

  // Setup the ip and stack-home for the new stack frame.
  *ip = assembly.getIp(tgtIpOffset);
  *sh = newSh;
  return true;
}

template <unsigned int StackSize>
inline auto callTail(
    Stack<StackSize>& stack,
    const Assembly& assembly,
    const uint8_t** ip,
    Value* sh,
    uint8_t argCount,
    uint32_t tgtIpOffset) -> void {

  /* In case of a tail-call we discard our current stack-frame, we copy the arguments to the
  beginning of the current-stack frame and update the ip. */

  // Move the arguments to the beginning of the current stack home.
  std::memcpy(sh, stack.getNext() - argCount, sizeof(Value) * argCount);

  stack.rewindToNext(sh + argCount); // Discard any extra values on the stack.
  *ip = assembly.getIp(tgtIpOffset);
}

template <unsigned int StackSize>
inline auto pushClosure(
    Stack<StackSize>& stack, const Value& closureVal, uint8_t* boundArgCount, uint32_t* ipOffset)
    -> bool {

  auto* closureStruct = getStructRef(closureVal);
  *boundArgCount      = closureStruct->getFieldCount() - 1U;
  assert(closureStruct->getFieldCount() > 0);

  // Push all bound arguments on the stack.
  bool success = true;
  for (auto i = 0U; i != *boundArgCount; ++i) {
    success &= stack.push(closureStruct->getField(i));
  }

  *ipOffset = closureStruct->getField(*boundArgCount).getUInt();
  return success;
}

template <typename PlatformInterface>
auto execute(
    Allocator* allocator,
    const Assembly& assembly,
    PlatformInterface& iface,
    uint32_t entryPoint,
    Value* execRetVal) noexcept -> ExecState {

  auto stack     = Stack<StackSize>{};
  auto execState = ExecState::Running;

#define READ_BYTE() readAsm<uint8_t>(&ip)
#define READ_INT() readAsm<int32_t>(&ip)
#define READ_UINT() readAsm<uint32_t>(&ip)
#define READ_FLOAT() readAsm<float>(&ip)
#define SALLOC(COUNT)                                                                              \
  if (!stack.alloc(COUNT)) {                                                                       \
    execState = ExecState::StackOverflow;                                                          \
    goto End;                                                                                      \
  }
#define PUSH(VAL)                                                                                  \
  if (!stack.push(VAL)) {                                                                          \
    execState = ExecState::StackOverflow;                                                          \
    goto End;                                                                                      \
  }
#define PUSH_UINT(VAL) PUSH(uintValue(VAL))
#define PUSH_INT(VAL) PUSH(intValue(VAL))
#define PUSH_FLOAT(VAL) PUSH(floatValue(VAL))
#define PUSH_REF(VAL) PUSH(refValue(VAL))
#define PUSH_CLOSURE(VAL, RES_BOUND_ARG_COUNT, RES_TGT_IP_OFFSET)                                  \
  if (!pushClosure(stack, VAL, RES_BOUND_ARG_COUNT, RES_TGT_IP_OFFSET)) {                          \
    execState = ExecState::StackOverflow;                                                          \
    goto End;                                                                                      \
  }
#define POP() stack.pop()
#define POP_UINT() POP().getUInt()
#define POP_INT() POP().getInt()
#define POP_FLOAT() POP().getFloat()
#define CALL(ARG_COUNT, TGT_IP)                                                                    \
  if (!call(stack, assembly, &ip, &sh, ARG_COUNT, TGT_IP)) {                                       \
    execState = ExecState::StackOverflow; /* Can only fail beccause of a stack-overflow atm. */    \
    goto End;                                                                                      \
  }
#define CALL_TAIL(ARG_COUNT, TGT_IP) callTail(stack, assembly, &ip, sh, ARG_COUNT, TGT_IP);

  const uint8_t* ip = assembly.getIp(entryPoint);
  Value* sh =
      stack.getNext(); // Note: Starts of pointing to an invalid stack entry as its still empty.
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
    case OpCode::LoadLitFloat: {
      PUSH_FLOAT(READ_FLOAT());
    } break;
    case OpCode::LoadLitString: {
      const auto& litStr = assembly.getLitString(READ_INT());
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
    case OpCode::AddFloat: {
      PUSH_FLOAT(POP_FLOAT() + POP_FLOAT());
    } break;
    case OpCode::AddString: {
      auto* b = getStringRef(POP());
      auto* a = getStringRef(POP());
      PUSH_REF(concatString(allocator, a, b));
    } break;
    case OpCode::SubInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a - b);
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
    case OpCode::MulFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(a * b);
    } break;
    case OpCode::DivInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      if (b == 0) {
        execState = ExecState::DivByZero;
        goto End;
      }
      PUSH_INT(a / b);
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
        execState = ExecState::DivByZero;
        goto End;
      }
      PUSH_INT(a % b);
    } break;
    case OpCode::ModFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(std::fmodf(a, b));
    } break;
    case OpCode::PowFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(std::powf(a, b));
    } break;
    case OpCode::SqrtFloat: {
      PUSH_FLOAT(std::sqrtf(POP_FLOAT()));
    } break;
    case OpCode::SinFloat: {
      PUSH_FLOAT(std::sinf(POP_FLOAT()));
    } break;
    case OpCode::CosFloat: {
      PUSH_FLOAT(std::cosf(POP_FLOAT()));
    } break;
    case OpCode::TanFloat: {
      PUSH_FLOAT(std::tanf(POP_FLOAT()));
    } break;
    case OpCode::ASinFloat: {
      PUSH_FLOAT(std::asinf(POP_FLOAT()));
    } break;
    case OpCode::ACosFloat: {
      PUSH_FLOAT(std::acosf(POP_FLOAT()));
    } break;
    case OpCode::ATanFloat: {
      PUSH_FLOAT(std::atanf(POP_FLOAT()));
    } break;
    case OpCode::ATan2Float: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_FLOAT(std::atan2f(a, b));
    } break;
    case OpCode::NegInt: {
      PUSH_INT(-POP_INT());
    } break;
    case OpCode::NegFloat: {
      PUSH_FLOAT(-POP_FLOAT());
    } break;
    case OpCode::LogicInvInt: {
      PUSH_INT(POP_INT() == 0 ? 1 : 0);
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
      PUSH_INT(a == b ? 1 : 0);
    } break;
    case OpCode::CheckEqFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_INT(a == b ? 1 : 0);
    } break;
    case OpCode::CheckEqString: {
      auto* b = getStringRef(POP());
      auto* a = getStringRef(POP());
      PUSH_INT(checkStringEq(a, b) ? 1 : 0);
    } break;
    case OpCode::CheckEqIp: {
      auto b = POP_UINT();
      auto a = POP_UINT();
      PUSH_INT(a == b ? 1 : 0);
    } break;
    case OpCode::CheckEqCallDynTgt: {
      // Compare the target instruction pointers (which for closure structs are stored in the last
      // field). Note: This does not compare bound arguments in a closure struct, main reason is
      // that we have no type information for those.
      auto b   = POP();
      auto bIp = (b.isRef() ? getStructRef(b)->getLastField() : b).getUInt();
      auto a   = POP();
      auto aIp = (a.isRef() ? getStructRef(a)->getLastField() : a).getUInt();
      PUSH_INT(aIp == bIp ? 1 : 0);
    } break;
    case OpCode::CheckGtInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a > b ? 1 : 0);
    } break;
    case OpCode::CheckGtFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_INT(a > b ? 1 : 0);
    } break;
    case OpCode::CheckLeInt: {
      auto b = POP_INT();
      auto a = POP_INT();
      PUSH_INT(a < b ? 1 : 0);
    } break;
    case OpCode::CheckLeFloat: {
      auto b = POP_FLOAT();
      auto a = POP_FLOAT();
      PUSH_INT(a < b ? 1 : 0);
    } break;

    case OpCode::ConvIntFloat: {
      PUSH_FLOAT(static_cast<float>(POP_INT()));
    } break;
    case OpCode::ConvFloatInt: {
      PUSH_INT(static_cast<int>(POP_FLOAT()));
    } break;
    case OpCode::ConvIntString: {
      PUSH_REF(toString(allocator, POP_INT()));
    } break;
    case OpCode::ConvFloatString: {
      PUSH_REF(toString(allocator, POP_FLOAT()));
    } break;
    case OpCode::ConvCharString: {
      PUSH_REF(toString(allocator, static_cast<uint8_t>(POP_INT())));
    } break;
    case OpCode::ConvIntChar: {
      PUSH_INT(static_cast<uint8_t>(POP_INT()));
    } break;

    case OpCode::MakeStruct: {
      const auto fieldCount = READ_BYTE();
      assert(fieldCount > 0);

      auto structRefAlloc = allocator->allocStruct(fieldCount);
      // Important to iterate in reverse, as the fields are in reverse order on the stack.
      for (auto fieldIndex = fieldCount; fieldIndex-- > 0;) {
        *(structRefAlloc.second + fieldIndex) = POP();
      }
      PUSH_REF(structRefAlloc.first);
    } break;
    case OpCode::LoadStructField: {
      const auto fieldIndex = READ_BYTE();
      auto s                = getStructRef(POP());
      PUSH(s->getField(fieldIndex));
    } break;

    case OpCode::Jump: {
      ip = assembly.getIp(READ_UINT());
    } break;
    case OpCode::JumpIf: {
      auto ipOffset = READ_UINT();
      if (POP_INT() != 0) {
        ip = assembly.getIp(ipOffset);
      }
    } break;

    case OpCode::Call: {
      const auto argCount    = READ_BYTE();
      const auto tgtIpOffset = READ_UINT();
      CALL(argCount, tgtIpOffset);
    } break;
    case OpCode::CallTail: {
      const auto argCount    = READ_BYTE();
      const auto tgtIpOffset = READ_UINT();
      CALL_TAIL(argCount, tgtIpOffset);
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
    case OpCode::PCall: {

      pcall(stack, allocator, iface, readAsm<PCallCode>(&ip), &execState);
      if (execState != ExecState::Running) {
        assert(execState != ExecState::Success);
        goto End;
      }
    } break;
    case OpCode::Ret: {
      /* Restore the previous instruction pointer and stack-home from the stack-frame meta-data. */
      auto retVal = POP();

      if (sh == stack.getBottom()) {
        *execRetVal = retVal;
        execState   = ExecState::Success;
        goto End; // Execution finishes after we returned from the last stack-frame.
      }
      assert(stack.getSize() >= 2);

      stack.rewindToNext(sh - 2); // Rewind this entire stack-frame (+ 2 for the meta-data).

      // Note this assumes that the rewinding does not actually invalidate the memory (which it
      // doesn't).
      ip = assembly.getIp((sh - 2)->getUInt());
      sh = (sh - 1)->getRawPtr<Value>();

      // Place the return-value on the stack.
      PUSH(retVal);
    } break;

    case OpCode::Dup:
      PUSH(stack.peek());
      break;
    case OpCode::Pop:
      POP();
      break;

    case OpCode::Fail:
    default:
      execState = ExecState::InvalidAssembly;
      goto End;
    }
  }

#undef READ_BYTE
#undef READ_INT
#undef READ_UINT
#undef READ_FLOAT
#undef SALLOC
#undef PUSH
#undef PUSH_UINT
#undef PUSH_INT
#undef PUSH_FLOAT
#undef PUSH_REF
#undef PUSH_CLOSURE
#undef POP
#undef POP_UINT
#undef POP_INT
#undef POP_FLOAT
#undef CALL
#undef CALL_TAIL
End:
  return execState;
}

// Explicit instantiations.
template ExecState execute(
    Allocator* allocator,
    const Assembly& assembly,
    platform::MemoryInterface& iface,
    uint32_t entryPoint,
    Value* execRetVal) noexcept;
template ExecState execute(
    Allocator* allocator,
    const Assembly& assembly,
    platform::TerminalInterface& iface,
    uint32_t entryPoint,
    Value* execRetVal) noexcept;

} // namespace vm::internal
