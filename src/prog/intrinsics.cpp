#include "prog/intrinsics.hpp"
#include "prog/operator.hpp"
#include <stdexcept>

namespace prog {

auto registerIntrinsics(
    sym::TypeDeclTable* typeTable, sym::FuncDeclTable* funcTable, sym::ActionDeclTable* actionTable)
    -> void {
  if (typeTable == nullptr) {
    throw std::invalid_argument{"Given typeTable is null"};
  }
  if (funcTable == nullptr) {
    throw std::invalid_argument{"Given funcTable is null"};
  }

  // Register types.
  const auto intT  = typeTable->registerIntrinsic("int");
  const auto boolT = typeTable->registerIntrinsic("bool");

  // Register unary int operators.
  funcTable->registerIntrinsic(getFuncName(Operator::Minus), sym::FuncSig{sym::Input{intT}, intT});

  // Register binary int operators.
  funcTable->registerIntrinsic(
      getFuncName(Operator::Minus), sym::FuncSig{sym::Input{intT, intT}, intT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::Plus), sym::FuncSig{sym::Input{intT, intT}, intT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::Star), sym::FuncSig{sym::Input{intT, intT}, intT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::Slash), sym::FuncSig{sym::Input{intT, intT}, intT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::EqEq), sym::FuncSig{sym::Input{intT, intT}, boolT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::BangEq), sym::FuncSig{sym::Input{intT, intT}, boolT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::Less), sym::FuncSig{sym::Input{intT, intT}, boolT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::LessEq), sym::FuncSig{sym::Input{intT, intT}, boolT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::Gt), sym::FuncSig{sym::Input{intT, intT}, boolT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::GtEq), sym::FuncSig{sym::Input{intT, intT}, boolT});

  // Register unary bool operators.
  funcTable->registerIntrinsic(getFuncName(Operator::Bang), sym::FuncSig{sym::Input{boolT}, boolT});

  // Register binary bool operators.
  funcTable->registerIntrinsic(
      getFuncName(Operator::EqEq), sym::FuncSig{sym::Input{boolT, boolT}, boolT});
  funcTable->registerIntrinsic(
      getFuncName(Operator::BangEq), sym::FuncSig{sym::Input{boolT, boolT}, boolT});

  // Register actions.
  actionTable->registerIntrinsic("print", sym::Input{intT});
  actionTable->registerIntrinsic("print", sym::Input{boolT});
}

} // namespace prog
