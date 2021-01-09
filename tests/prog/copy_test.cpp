#include "catch2/catch.hpp"
#include "prog/copy.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/program.hpp"
#include "prog/sym/const_decl_table.hpp"

namespace prog {

TEST_CASE("[prog] Copy", "prog") {

  SECTION("Copy type") {
    auto progA         = Program{};
    const auto id      = progA.declareStruct("test");
    auto fields        = sym::FieldDeclTable{};
    const auto fieldId = fields.registerField("field1", progA.getInt());
    progA.defineStruct(id, fields);

    // Copy to another program.
    auto progB = Program{};
    copyType(progA, &progB, id);

    // Assert its presence.
    REQUIRE(progB.lookupType("test") == id);
    REQUIRE(std::holds_alternative<sym::StructDef>(progB.getTypeDef(id)));
    REQUIRE(std::get<sym::StructDef>(progB.getTypeDef(id)).getFields().lookup("field1") == fieldId);
  }

  SECTION("Copy function") {
    auto progA    = Program{};
    const auto id = progA.declarePureFunc("test", sym::TypeSet{}, progA.getInt());
    progA.defineFunc(id, sym::ConstDeclTable{}, expr::litIntNode(progA, 2));

    // Copy to another program.
    auto progB = Program{};
    copyFunc(progA, &progB, id);

    // Assert its presence.
    REQUIRE(progB.lookupFunc("test", sym::TypeSet{}, OvOptions{}) == id);
    REQUIRE(progB.getFuncDef(id).getExpr() == *expr::litIntNode(progB, 2));
  }

  SECTION("Fail to copy build-in type") {
    auto progA = Program{};
    auto progB = Program{};
    REQUIRE(!copyType(progA, &progB, progA.getInt()));
  }

  SECTION("Fail to copy intrinsic") {
    auto progA     = Program{};
    auto progB     = Program{};
    auto intrinsic = progA.lookupIntrinsic("clock_nanosteady", sym::TypeSet{}, OvOptions{});
    REQUIRE(intrinsic);
    REQUIRE(!copyFunc(progA, &progB, *intrinsic));
  }
}

} // namespace prog
