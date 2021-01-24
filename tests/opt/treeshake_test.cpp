#include "catch2/catch.hpp"
#include "opt/opt.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/program.hpp"

namespace opt {

TEST_CASE("[opt] Treeshake", "opt") {

  SECTION("Unused func") {
    auto prog = prog::Program{};
    auto funcId =
        prog.declarePureFunc("unused", prog::sym::TypeSet{prog.getInt()}, prog.getBool(), 0u);
    prog.defineFunc(funcId, prog::sym::ConstDeclTable{}, prog::expr::litBoolNode(prog, false), {});

    auto shakedProg = treeshake(prog);
    CHECK(prog.lookupFunc("unused", prog::sym::TypeSet{prog.getInt()}, prog::OvOptions{}));
    CHECK(!shakedProg.lookupFunc("unused", prog::sym::TypeSet{prog.getInt()}, prog::OvOptions{}));
  }

  SECTION("Used func") {
    auto prog   = prog::Program{};
    auto funcId = prog.declarePureFunc("used", prog::sym::TypeSet{}, prog.getBool(), 0u);
    prog.defineFunc(funcId, prog::sym::ConstDeclTable{}, prog::expr::litBoolNode(prog, false), {});
    prog.addExecStmt(prog::sym::ConstDeclTable{}, prog::expr::callExprNode(prog, funcId, {}));

    auto shakedProg = treeshake(prog);
    CHECK(prog.lookupFunc("used", prog::sym::TypeSet{}, prog::OvOptions{}));
    CHECK(shakedProg.lookupFunc("used", prog::sym::TypeSet{}, prog::OvOptions{}));
  }

  SECTION("Unused type") {
    auto prog   = prog::Program{};
    auto typeId = prog.declareStruct("unused");
    prog.defineStruct(typeId, prog::sym::FieldDeclTable{});

    auto shakedProg = treeshake(prog);
    CHECK(prog.lookupType("unused"));
    CHECK(!shakedProg.lookupType("unused"));
  }

  SECTION("Used type") {
    auto prog   = prog::Program{};
    auto typeId = prog.declareStruct("used");
    prog.defineStruct(typeId, prog::sym::FieldDeclTable{});

    // Call constructor of struct.
    prog.addExecStmt(
        prog::sym::ConstDeclTable{},
        prog::expr::callExprNode(
            prog, *prog.lookupFunc("used", prog::sym::TypeSet{}, prog::OvOptions{}), {}));

    auto shakedProg = treeshake(prog);
    CHECK(prog.lookupType("used"));
    CHECK(shakedProg.lookupType("used"));
  }
}

} // namespace opt
