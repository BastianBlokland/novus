#include "vm/executor.hpp"
#include "internal/call_stack.hpp"
#include "internal/eval_stack.hpp"
#include "vm/exceptions/invalid_program.hpp"
#include "vm/opcode.hpp"
#include <iostream>

namespace vm {

static const int MaxEvalStackSize = 1024;

static auto execute(const Program& program, uint32_t entryPoint) {
  auto evalStack = internal::EvalStack{MaxEvalStackSize};
  auto callStack = internal::CallStack{};
  callStack.push(program, entryPoint);

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
      evalStack.push(internal::intValue(evalStack.pop().getInt() * evalStack.pop().getInt()));
    } break;
    case OpCode::DivInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
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
    case OpCode::CheckLessInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a < b ? 1 : 0));
    } break;

    case OpCode::PrintInt: {
      auto a = evalStack.pop().getInt();
      std::cout << a;
    } break;
    case OpCode::PrintLogic: {
      auto a = evalStack.pop().getInt();
      std::cout << (a == 0 ? "false" : "true");
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
      callStack.push(program, ipOffset);
    } break;
    case OpCode::Ret: {
      if (!callStack.pop()) {
        return; // Execution finishes after we returned from the last scope.
      }
    } break;

    case OpCode::Dup: {
      evalStack.push(evalStack.peek());
    } break;
    case OpCode::Pop: {
      evalStack.pop();
    } break;

    case OpCode::Nop: {
      break;
    }
    case OpCode::Fail:
    default:
      throw exceptions::InvalidProgram{};
    }
  }
}

auto execute(const Program& program) -> void {
  for (auto itr = program.beginEntryPoints(); itr != program.endEntryPoints(); ++itr) {
    execute(program, *itr);
  }
}

} // namespace vm
