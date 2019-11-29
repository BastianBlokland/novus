#pragma once
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"

namespace frontend {

#define SRC(INPUT) buildSource("test", std::string{INPUT}.begin(), std::string{INPUT}.end())

#define ANALYZE(INPUT) analyze(SRC(INPUT))

#define GET_TYPE_ID(OUTPUT, TYPE_NAME) OUTPUT.getProg().lookupType(TYPE_NAME).value()

#define GET_FUNC_ID(OUTPUT, FUNCNAME, ...)                                                         \
  OUTPUT.getProg().lookupFunc(FUNCNAME, prog::sym::Input{__VA_ARGS__}, 0).value()

#define GET_OP_ID(OUTPUT, OP, ...) GET_FUNC_ID(OUTPUT, getFuncName(OP), __VA_ARGS__)

#define GET_FUNC_DECL(OUTPUT, FUNCNAME, ...)                                                       \
  OUTPUT.getProg().getFuncDecl(GET_FUNC_ID(OUTPUT, FUNCNAME, __VA_ARGS__))

#define GET_FUNC_DEF(OUTPUT, FUNCNAME, ...)                                                        \
  OUTPUT.getProg().getFuncDef(GET_FUNC_ID(OUTPUT, FUNCNAME, __VA_ARGS__))

#define GET_ACTION_ID(OUTPUT, ACTIONNAME, ...)                                                     \
  OUTPUT.getProg().lookupAction(ACTIONNAME, prog::sym::Input{__VA_ARGS__}, 0).value()

#define GET_ACTION_DECL(OUTPUT, ACTIONNAME, ...)                                                   \
  OUTPUT.getProg().getActionDecl(GET_ACTION_ID(OUTPUT, ACTIONNAME, __VA_ARGS__))

#define GET_CONV(OUTPUT, FROM_TYPE, TO_TYPE)                                                       \
  OUTPUT.getProg().lookupConversion(GET_TYPE_ID(OUTPUT, FROM_TYPE), GET_TYPE_ID(OUTPUT, TO_TYPE))

#define CHECK_DIAG(INPUT, ...)                                                                     \
  {                                                                                                \
    const auto src    = SRC(INPUT);                                                                \
    const auto output = analyze(src);                                                              \
    const auto diags  = std::vector<frontend::Diag>{output.beginDiags(), output.endDiags()};       \
    const std::vector<frontend::Diag> expectedDiags = {__VA_ARGS__};                               \
    CHECK_THAT(diags, Catch::UnorderedEquals(expectedDiags));                                      \
  }

} // namespace frontend
