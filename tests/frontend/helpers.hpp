#pragma once
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "prog/expr/node_call.hpp"

namespace frontend {

#define SRC(INPUT)                                                                                 \
  buildSource("test", std::nullopt, std::string{INPUT}.begin(), std::string{INPUT}.end())

#define ANALYZE(INPUT) analyze(SRC(INPUT))

#define TYPE_EXISTS(OUTPUT, TYPE_NAME) OUTPUT.getProg().lookupType(TYPE_NAME).has_value()

#define FUNC_EXISTS(OUTPUT, FUNC_NAME) OUTPUT.getProg().hasFunc(FUNC_NAME)

#define GET_TYPE_ID(OUTPUT, TYPE_NAME) OUTPUT.getProg().lookupType(TYPE_NAME).value()

#define GET_FUNC_ID(OUTPUT, FUNCNAME, ...)                                                         \
  OUTPUT.getProg()                                                                                 \
      .lookupFunc(FUNCNAME, prog::sym::TypeSet{__VA_ARGS__}, prog::sym::OverloadOptions{0})        \
      .value()

#define GET_OP_ID(OUTPUT, OP, ...) GET_FUNC_ID(OUTPUT, getFuncName(OP), __VA_ARGS__)

#define GET_TYPE_DEF(OUTPUT, TYPENAME) OUTPUT.getProg().getTypeDef(GET_TYPE_ID(OUTPUT, TYPENAME))

#define GET_FUNC_DECL(OUTPUT, FUNCNAME, ...)                                                       \
  OUTPUT.getProg().getFuncDecl(GET_FUNC_ID(OUTPUT, FUNCNAME, __VA_ARGS__))

#define GET_FUNC_DEF(OUTPUT, FUNCNAME, ...)                                                        \
  OUTPUT.getProg().getFuncDef(GET_FUNC_ID(OUTPUT, FUNCNAME, __VA_ARGS__))

#define GET_ACTION_ID(OUTPUT, ACTIONNAME, ...)                                                     \
  OUTPUT.getProg().lookupAction(ACTIONNAME, prog::sym::TypeSet{__VA_ARGS__}, 0).value()

#define GET_ACTION_DECL(OUTPUT, ACTIONNAME, ...)                                                   \
  OUTPUT.getProg().getActionDecl(GET_ACTION_ID(OUTPUT, ACTIONNAME, __VA_ARGS__))

#define GET_CONV(OUTPUT, FROM_TYPE, TO_TYPE)                                                       \
  OUTPUT.getProg().lookupImplicitConv(GET_TYPE_ID(OUTPUT, FROM_TYPE), GET_TYPE_ID(OUTPUT, TO_TYPE))

#define CHECK_DIAG(INPUT, ...)                                                                     \
  {                                                                                                \
    const auto src    = SRC(INPUT);                                                                \
    const auto output = analyze(src);                                                              \
    const auto diags  = std::vector<frontend::Diag>{output.beginDiags(), output.endDiags()};       \
    const std::vector<frontend::Diag> expectedDiags = {__VA_ARGS__};                               \
    CHECK_THAT(diags, Catch::UnorderedEquals(expectedDiags));                                      \
  }

inline auto findAnonFunc(const Output& output, unsigned int num) -> prog::sym::FuncId {
  for (auto itr = output.getProg().beginFuncDecls(); itr != output.getProg().endFuncDecls();
       ++itr) {
    const auto isAnon = itr->getName().rfind("__anon_", 0) == 0;
    if (isAnon && num-- == 0) {
      return itr->getId();
    }
  }
  throw std::logic_error{"Unable to find anonymous function"};
}

inline auto findAnonFuncDef(const Output& output, unsigned int num) -> const prog::sym::FuncDef& {
  return output.getProg().getFuncDef(findAnonFunc(output, num));
}

inline auto applyConv(
    const Output& output,
    const std::string& fromType,
    const std::string& toType,
    prog::expr::NodePtr childExpr) -> prog::expr::NodePtr {
  auto conv     = GET_CONV(output, fromType, toType);
  auto convArgs = std::vector<prog::expr::NodePtr>{};
  convArgs.push_back(std::move(childExpr));

  return prog::expr::callExprNode(output.getProg(), *conv, std::move(convArgs));
}

} // namespace frontend
