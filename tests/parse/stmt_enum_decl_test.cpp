#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_enum_decl.hpp"
#include <optional>

namespace parse {

TEST_CASE("[parse] Parsing enum declaration statements", "parse") {

  CHECK_STMT(
      "enum e = a",
      enumDeclStmtNode(
          ENUM, ID("e"), EQ, {EnumDeclStmtNode::EntrySpec{ID("a"), std::nullopt}}, COMMAS(0)));
  CHECK_STMT(
      "enum e = a, b",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{ID("a"), std::nullopt},
           EnumDeclStmtNode::EntrySpec{ID("b"), std::nullopt}},
          COMMAS(1)));
  CHECK_STMT(
      "enum e = a : 1",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{
              ID("a"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(1)}}},
          COMMAS(0)));
  CHECK_STMT(
      "enum e = a : 1, b : 2",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{
               ID("a"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(1)}},
           EnumDeclStmtNode::EntrySpec{
               ID("b"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(2)}}},
          COMMAS(1)));
  CHECK_STMT(
      "enum e = a : 1, b : 1",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{
               ID("a"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(1)}},
           EnumDeclStmtNode::EntrySpec{
               ID("b"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(1)}}},
          COMMAS(1)));
  CHECK_STMT(
      "enum e = a, b : 2",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{ID("a"), std::nullopt},
           EnumDeclStmtNode::EntrySpec{
               ID("b"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(2)}}},
          COMMAS(1)));
  CHECK_STMT(
      "enum e = a : 1, b",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{
               ID("a"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(1)}},
           EnumDeclStmtNode::EntrySpec{ID("b"), std::nullopt}},
          COMMAS(1)));
  CHECK_STMT(
      "enum e = a : -1, b",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{
               ID("a"), EnumDeclStmtNode::ValueSpec{COLON, MINUS, INT_TOK(1)}},
           EnumDeclStmtNode::EntrySpec{ID("b"), std::nullopt}},
          COMMAS(1)));
  CHECK_STMT(
      "enum e = a : -1, b : -999",
      enumDeclStmtNode(
          ENUM,
          ID("e"),
          EQ,
          {EnumDeclStmtNode::EntrySpec{
               ID("a"), EnumDeclStmtNode::ValueSpec{COLON, MINUS, INT_TOK(1)}},
           EnumDeclStmtNode::EntrySpec{
               ID("b"), EnumDeclStmtNode::ValueSpec{COLON, MINUS, INT_TOK(999)}}},
          COMMAS(1)));

  SECTION("Errors") {
    CHECK_STMT("enum", errInvalidStmtEnumDecl(ENUM, END, END, {}, COMMAS(0)));
    CHECK_STMT("enum e", errInvalidStmtEnumDecl(ENUM, ID("e"), END, {}, COMMAS(0)));
    CHECK_STMT("enum e =", errInvalidStmtEnumDecl(ENUM, ID("e"), EQ, {}, COMMAS(0)));
    CHECK_STMT(
        "enum e = a,",
        errInvalidStmtEnumDecl(
            ENUM, ID("e"), EQ, {EnumDeclStmtNode::EntrySpec{ID("a"), std::nullopt}}, COMMAS(1)));
    CHECK_STMT(
        "enum e = a :",
        errInvalidStmtEnumDecl(
            ENUM,
            ID("e"),
            EQ,
            {EnumDeclStmtNode::EntrySpec{
                ID("a"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, END}}},
            COMMAS(0)));
    CHECK_STMT(
        "enum e = a : 1, b :",
        errInvalidStmtEnumDecl(
            ENUM,
            ID("e"),
            EQ,
            {EnumDeclStmtNode::EntrySpec{
                 ID("a"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, INT_TOK(1)}},
             EnumDeclStmtNode::EntrySpec{
                 ID("b"), EnumDeclStmtNode::ValueSpec{COLON, std::nullopt, END}}},
            COMMAS(1)));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" enum Color = Red, Blue ", input::Span(1, 22));
    CHECK_STMT_SPAN(" enum Permission = Read : 0b01, Write : 0b10 ", input::Span(1, 36));
  }
}

} // namespace parse
