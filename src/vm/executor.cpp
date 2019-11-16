#include "vm/executor.hpp"
#include "internal/call_stack.hpp"
#include "internal/eval_stack.hpp"
#include "vm/exceptions/div_by_zero.hpp"
#include "vm/exceptions/invalid_assembly.hpp"
#include "vm/exceptions/stack_not_empty.hpp"
#include "vm/opcode.hpp"
#include <stdexcept>

namespace vm {

static const int MaxEvalStackSize = 1024;

static auto execute(const Assembly& assembly, io::Interface* interface, uint32_t entryPoint) {
  auto evalStack = internal::EvalStack{MaxEvalStackSize};
  auto callStack = internal::CallStack{};
  callStack.push(assembly, entryPoint);

  while (true) {
    auto scope = callStack.getTop();
    switch (scope->readOpCode()) {
    case OpCode::LoadLitInt: {
      auto litInt = scope->readInt32();
      evalStack.push(internal::intValue(litInt));
    } break;

    case OpCode::StoreConst: {
      auto constId = scope->readUInt8();
      auto a       = evalStack.pop();
      scope->setConst(constId, a);
    } break;
    case OpCode::LoadConst: {
      auto constId = scope->readUInt8();
      evalStack.push(scope->getConst(constId));
    } break;

    case OpCode::AddInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a + b));
    } break;
    case OpCode::SubInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a - b));
    } break;
    case OpCode::MulInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a * b));
    } break;
    case OpCode::DivInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      if (b == 0) {
        throw exceptions::DivByZero{};
      }
      evalStack.push(internal::intValue(a / b));
    } break;
    case OpCode::NegInt: {
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(-a));
    } break;
    case OpCode::LogicInvInt: {
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a == 0 ? 1 : 0));
    } break;

    case OpCode::CheckEqInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a == b ? 1 : 0));
    } break;
    case OpCode::CheckGtInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a > b ? 1 : 0));
    } break;
    case OpCode::CheckLeInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a < b ? 1 : 0));
    } break;

    case OpCode::PrintInt: {
      auto a = evalStack.pop().getInt();
      interface->print(std::to_string(a));
    } break;
    case OpCode::PrintLogic: {
      auto a = evalStack.pop().getInt();
      interface->print(a == 0 ? "false" : "true");
    } break;

    case OpCode::Jump: {
      auto ipOffset = scope->readUInt32();
      scope->jump(ipOffset);
    } break;
    case OpCode::JumpIf: {
      auto ipOffset = scope->readUInt32();
      auto a        = evalStack.pop().getInt();
      if (a != 0) {
        scope->jump(ipOffset);
      }
    } break;

    case OpCode::Call: {
      auto ipOffset = scope->readUInt32();
      callStack.push(assembly, ipOffset);
    } break;
    case OpCode::Ret: {
      if (!callStack.pop()) {
        if (evalStack.getSize() != 0) {
          throw exceptions::StackNotEmpty{};
        }
        return; // Execution finishes after we returned from the last scope.
      }
    } break;

    case OpCode::Fail:
    default:
      throw exceptions::InvalidAssembly{};
    }
  }
}

auto execute(const Assembly& assembly, io::Interface* interface) -> void {
  if (!interface) {
    throw std::invalid_argument{"Interface cannot be null"};
  }
  for (auto itr = assembly.beginEntryPoints(); itr != assembly.endEntryPoints(); ++itr) {
    execute(assembly, interface, *itr);
  }
}

} // namespace vm
