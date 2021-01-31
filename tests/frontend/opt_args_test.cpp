#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_char.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing optional argument", "frontend") {

  SECTION("Function") {

    SECTION("Declare function with single optional argument") {
      const auto& output = ANALYZE("fun f(int a = 0) false");
      REQUIRE(output.isSuccess());

      const auto& funcDecl = GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getMinInputCount() == 0);

      const auto& funcDef = output.getProg().getFuncDef(funcDecl.getId());
      CHECK(*funcDef.getOptArgInitializer(0) == *prog::expr::litIntNode(output.getProg(), 0));
    }

    SECTION("Declare a function with a normal argument and a optional argument") {
      const auto& output = ANALYZE("fun f(float a, int b = 0) a + b");
      REQUIRE(output.isSuccess());

      const auto& funcDecl =
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "float"), GET_TYPE_ID(output, "int"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getMinInputCount() == 1);

      const auto& funcDef = output.getProg().getFuncDef(funcDecl.getId());
      CHECK(*funcDef.getOptArgInitializer(0) == *prog::expr::litIntNode(output.getProg(), 0));
    }

    SECTION("Declare a function with a conversion on the optional arg initializer") {
      const auto& output = ANALYZE("fun f(float a = 2) false");
      REQUIRE(output.isSuccess());

      const auto& funcDecl = GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "float"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getMinInputCount() == 0);

      const auto& funcDef = output.getProg().getFuncDef(funcDecl.getId());
      CHECK(
          *funcDef.getOptArgInitializer(0) ==
          *applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)));
    }

    SECTION("Declare a function with a call on the initializer") {
      const auto& output = ANALYZE("fun fa() 42 "
                                   "fun fb(int i = fa()) i");
      REQUIRE(output.isSuccess());

      const auto& funcDecl = GET_FUNC_DECL(output, "fb", GET_TYPE_ID(output, "int"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getMinInputCount() == 0);

      const auto& funcDef = output.getProg().getFuncDef(funcDecl.getId());
      CHECK(
          *funcDef.getOptArgInitializer(0) ==
          *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "fa"), {}));
    }

    SECTION("Declare a function with an optional arg initializer that calls an intrinsic") {
      const auto& output = ANALYZE("act a(long time = intrinsic{clock_nanosteady}()) time * 2");
      REQUIRE(output.isSuccess());

      const auto& funcDecl = GET_FUNC_DECL(output, "a", GET_TYPE_ID(output, "long"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getMinInputCount() == 0);

      const auto& funcDef = output.getProg().getFuncDef(funcDecl.getId());
      CHECK(
          *funcDef.getOptArgInitializer(0) ==
          *prog::expr::callExprNode(
              output.getProg(), GET_INTRINSIC_ID(output, "clock_nanosteady"), {}));
    }

    SECTION("Call a function with an optional argument") {
      const auto& output = ANALYZE("fun fa(int a, int b = 1 + 2) a + b "
                                   "fun fb() fa(2)");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DEF(output, "fb").getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              GET_FUNC_ID(output, "fa", GET_TYPE_ID(output, "int")),
              EXPRS(
                  prog::expr::litIntNode(output.getProg(), 2),
                  prog::expr::callExprNode(
                      output.getProg(),
                      GET_OP_ID(
                          output,
                          prog::Operator::Plus,
                          GET_TYPE_ID(output, "int"),
                          GET_TYPE_ID(output, "int")),
                      EXPRS(
                          prog::expr::litIntNode(output.getProg(), 1),
                          prog::expr::litIntNode(output.getProg(), 2))))));
    }

    SECTION("Call a function with an optional argument with a conversion") {
      const auto& output = ANALYZE("fun fa(int a, float b = 2) a + b "
                                   "fun fb() fa(2)");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DEF(output, "fb").getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              GET_FUNC_ID(output, "fa", GET_TYPE_ID(output, "int")),
              EXPRS(
                  prog::expr::litIntNode(output.getProg(), 2),
                  applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)))));
    }

    SECTION("Diagnostics") {

      CHECK_DIAG(
          "fun f(int a = \"Hello World\") a",
          errNonMatchingInitializerType(src, "int", "string", input::Span{14, 26}));
      CHECK_DIAG(
          "struct S{T} = T t "
          "fun f(S{int} a = S(\"Hello World\")) a",
          errNonMatchingInitializerType(src, "S{int}", "S{string}", input::Span{35, 50}));

      CHECK_DIAG("fun f(int a = (b = 2)) a", errConstDeclareNotSupported(src, input::Span{15, 19}));
      CHECK_DIAG(
          "fun f(int a = (b = 2; b * 2)) a",
          errConstDeclareNotSupported(src, input::Span{15, 19}),
          errUndeclaredConst(src, "b", input::Span{22, 22}));

      CHECK_DIAG(
          "fun fb(int a = fb()) a", errUndeclaredPureFunc(src, "fb", {}, input::Span{15, 18}));
      CHECK_DIAG(
          "fun fa(int a = 0) a "
          "fun fb(int a = fa()) a",
          errUndeclaredPureFunc(src, "fa", {}, input::Span{35, 38}));
      CHECK_DIAG(
          "fun fa(int a = fb()) a "
          "fun fb(int a = fa()) a",
          errUndeclaredPureFunc(src, "fb", {}, input::Span{15, 18}),
          errUndeclaredPureFunc(src, "fa", {}, input::Span{38, 41}));

      CHECK_DIAG(
          "fun f(int a = 0, int b) a + b", errNonOptArgFollowingOpt(src, input::Span{17, 21}));
      CHECK_DIAG(
          "fun f(int a = 0, int b, int c = 0) a + b + c",
          errNonOptArgFollowingOpt(src, input::Span{17, 21}));
      CHECK_DIAG(
          "fun f(int a, int b = 0, int c) a + b + c",
          errNonOptArgFollowingOpt(src, input::Span{24, 28}));

      CHECK_DIAG(
          "act a() 42 "
          "fun f(int i = a()) i",
          errUndeclaredPureFunc(src, "a", {}, input::Span{25, 27}));
    }
  }

  SECTION("Templated function") {

    SECTION("Declare a templated function with a single optional argument") {
      const auto& output = ANALYZE("fun ft{T}(T a = 0) a "
                                   "fun f() ft{int}()");
      REQUIRE(output.isSuccess());

      const auto& funcDecl = GET_FUNC_DECL(output, "ft__int", GET_TYPE_ID(output, "int"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getMinInputCount() == 0);

      const auto& funcDef = output.getProg().getFuncDef(funcDecl.getId());
      CHECK(*funcDef.getOptArgInitializer(0) == *prog::expr::litIntNode(output.getProg(), 0));
    }

    SECTION("Type parameter can be inferred from an initializer") {
      const auto& output = ANALYZE("fun ft{TX, TY}(TX a = \"World\", TY b = 'W') a + string(b) "
                                   "fun f1() ft() "
                                   "fun f2() ft(\"Hello\", 42) "
                                   "fun f3() ft(\"Hello\")");
      REQUIRE(output.isSuccess());

      const auto& funcT1Decl = GET_FUNC_DECL(
          output, "ft__string_char", GET_TYPE_ID(output, "string"), GET_TYPE_ID(output, "char"));
      CHECK(funcT1Decl.getNumOptInputs() == 2);
      CHECK(funcT1Decl.getMinInputCount() == 0);

      const auto& funcT2Decl = GET_FUNC_DECL(
          output, "ft__string_int", GET_TYPE_ID(output, "string"), GET_TYPE_ID(output, "int"));
      CHECK(funcT2Decl.getNumOptInputs() == 2);
      CHECK(funcT2Decl.getMinInputCount() == 0);

      CHECK(
          GET_FUNC_DEF(output, "f1").getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              funcT1Decl.getId(),
              EXPRS(
                  prog::expr::litStringNode(output.getProg(), "World"),
                  prog::expr::litCharNode(output.getProg(), 'W'))));

      CHECK(
          GET_FUNC_DEF(output, "f2").getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              funcT2Decl.getId(),
              EXPRS(
                  prog::expr::litStringNode(output.getProg(), "Hello"),
                  prog::expr::litIntNode(output.getProg(), 42))));

      CHECK(
          GET_FUNC_DEF(output, "f3").getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              funcT1Decl.getId(),
              EXPRS(
                  prog::expr::litStringNode(output.getProg(), "Hello"),
                  prog::expr::litCharNode(output.getProg(), 'W'))));
    }

    SECTION("Diagnostics") {

      CHECK_DIAG(
          "fun ft{T}(T a = \"Hello World\") a "
          "fun f() ft{int}()",
          errNonMatchingInitializerType(src, "int", "string", input::Span{16, 28}),
          errInvalidFuncInstantiation(src, input::Span{41, 42}),
          errNoPureFuncFoundToInstantiate(src, "ft", 1, input::Span{41, 49}));

      CHECK_DIAG(
          "fun ft{T}(T a = (b = 2)) a "
          "fun f() ft{int}()",
          errConstDeclareNotSupported(src, input::Span{17, 21}),
          errInvalidFuncInstantiation(src, input::Span{35, 36}),
          errNoPureFuncFoundToInstantiate(src, "ft", 1, input::Span{35, 43}));

      CHECK_DIAG(
          "fun ft{T}(T a = ft()) a "
          "fun f() ft{int}()",
          errUndeclaredPureFunc(src, "ft", {}, input::Span{16, 19}),
          errInvalidFuncInstantiation(src, input::Span{32, 33}),
          errNoPureFuncFoundToInstantiate(src, "ft", 1, input::Span{32, 40}));

      CHECK_DIAG(
          "fun ft{T}(T a = T(), T b) a + b "
          "fun f() ft{int}()",
          errNonOptArgFollowingOpt(src, input::Span{21, 23}),
          errNoPureFuncFoundToInstantiate(src, "ft", 1, input::Span{40, 48}));

      CHECK_DIAG(
          "fun f(int a = 0, int b) a + b", errNonOptArgFollowingOpt(src, input::Span{17, 21}));
    }
  }

  SECTION("Lambda") {

    SECTION("Diagnostics") {

      CHECK_DIAG(
          "fun f() lambda (int i = 0) i",
          errUnsupportedArgInitializer(src, "i", input::Span{16, 24}));
      CHECK_DIAG(
          "fun f() lambda (int a = 0, int b = 0) a + b",
          errUnsupportedArgInitializer(src, "a", input::Span{16, 24}),
          errUnsupportedArgInitializer(src, "b", input::Span{27, 35}));
    }
  }
}

} // namespace frontend
