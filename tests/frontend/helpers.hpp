#pragma once
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "prog/expr/node_call.hpp"
#include <algorithm>
#include <array>
#include <vector>

namespace frontend {

#define NUM_ARGS(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

inline auto buildSource(std::string input) {
  return buildSource("test", std::nullopt, input.begin(), input.end());
}

#define SRC(INPUT) buildSource(std::string{INPUT})

#define ANALYZE(INPUT) analyze(SRC(INPUT))

#define TYPE_EXISTS(OUTPUT, TYPE_NAME) OUTPUT.getProg().lookupType(TYPE_NAME).has_value()

#define FUNC_EXISTS(OUTPUT, FUNC_NAME) OUTPUT.getProg().hasFunc(FUNC_NAME)

#define GET_TYPE_ID(OUTPUT, TYPE_NAME) OUTPUT.getProg().lookupType(TYPE_NAME).value()

#define GET_FUNC_ID(OUTPUT, FUNCNAME, ...)                                                         \
  OUTPUT.getProg()                                                                                 \
      .lookupFunc(FUNCNAME, prog::sym::TypeSet{__VA_ARGS__}, prog::sym::OverloadOptions{0})        \
      .value()

#define GET_INTRINSIC_ID(OUTPUT, INTRINSIC_NAME, ...)                                              \
  OUTPUT.getProg()                                                                                 \
      .lookupIntrinsic(                                                                            \
          INTRINSIC_NAME, prog::sym::TypeSet{__VA_ARGS__}, prog::sym::OverloadOptions{0})          \
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
    const auto output = analyze(SRC(INPUT));                                                       \
    const auto diags  = std::vector<frontend::Diag>{output.beginDiags(), output.endDiags()};       \
    const std::vector<frontend::Diag> expectedDiags = {__VA_ARGS__};                               \
    CHECK_THAT(diags, Catch::UnorderedEquals(expectedDiags));                                      \
  }

inline auto findAnonFunc(const Output& output, unsigned int num) -> prog::sym::FuncId {
  auto anonFuncIds = std::vector<prog::sym::FuncId>{};
  for (auto itr = output.getProg().beginFuncDecls(); itr != output.getProg().endFuncDecls();
       ++itr) {
    if (itr->second.getName().rfind("__anon_", 0) == 0) {
      anonFuncIds.push_back(itr->first);
    }
  }
  std::sort(anonFuncIds.begin(), anonFuncIds.end());

  if (num >= anonFuncIds.size()) {
    throw std::logic_error{"Unable to find anonymous function"};
  }
  return anonFuncIds[num];
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

template <typename Array>
inline auto arrayMoveToVec(Array c) {
  auto result = std::vector<typename Array::value_type>{};
  for (auto& elem : c) {
    result.push_back(std::move(elem));
  }
  return result;
}

#define EXPRS(...)                                                                                 \
  arrayMoveToVec<std::array<prog::expr::NodePtr, NUM_ARGS(__VA_ARGS__)>>({__VA_ARGS__})

#define NO_EXPRS                                                                                   \
  std::vector<prog::expr::NodePtr> {}

#define NO_SRC prog::sym::SourceId::none()

} // namespace frontend
