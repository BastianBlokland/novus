#include "internal/intrinsics.hpp"
#include "internal/expr_matchers.hpp"
#include "internal/utilities.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_char.hpp"
#include "prog/expr/node_lit_enum.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_long.hpp"
#include "prog/expr/node_lit_string.hpp"
#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace opt::internal {

auto isPrecomputableIntrinsic(prog::sym::FuncKind funcKind) -> bool {
  switch (funcKind) {
  // Int
  case prog::sym::FuncKind::AddInt:
  case prog::sym::FuncKind::SubInt:
  case prog::sym::FuncKind::MulInt:
  case prog::sym::FuncKind::DivInt:
  case prog::sym::FuncKind::RemInt:
  case prog::sym::FuncKind::NegateInt:
  case prog::sym::FuncKind::ShiftLeftInt:
  case prog::sym::FuncKind::ShiftRightInt:
  case prog::sym::FuncKind::AndInt:
  case prog::sym::FuncKind::OrInt:
  case prog::sym::FuncKind::XorInt:
  case prog::sym::FuncKind::InvInt:
  case prog::sym::FuncKind::CheckEqInt:
  case prog::sym::FuncKind::CheckLeInt:
  case prog::sym::FuncKind::CheckLeEqInt:
  case prog::sym::FuncKind::CheckGtInt:
  case prog::sym::FuncKind::CheckGtEqInt:
  case prog::sym::FuncKind::CheckIntZero:
  case prog::sym::FuncKind::ConvIntLong:
  case prog::sym::FuncKind::ConvCharLong:
  case prog::sym::FuncKind::ConvIntFloat:
  case prog::sym::FuncKind::ConvIntString:
  case prog::sym::FuncKind::ConvIntChar:

  // Float
  case prog::sym::FuncKind::AddFloat:
  case prog::sym::FuncKind::SubFloat:
  case prog::sym::FuncKind::MulFloat:
  case prog::sym::FuncKind::DivFloat:
  case prog::sym::FuncKind::ModFloat:
  case prog::sym::FuncKind::PowFloat:
  case prog::sym::FuncKind::SqrtFloat:
  case prog::sym::FuncKind::SinFloat:
  case prog::sym::FuncKind::CosFloat:
  case prog::sym::FuncKind::TanFloat:
  case prog::sym::FuncKind::ASinFloat:
  case prog::sym::FuncKind::ACosFloat:
  case prog::sym::FuncKind::ATanFloat:
  case prog::sym::FuncKind::ATan2Float:
  case prog::sym::FuncKind::NegateFloat:
  case prog::sym::FuncKind::CheckEqFloat:
  case prog::sym::FuncKind::CheckLeFloat:
  case prog::sym::FuncKind::CheckLeEqFloat:
  case prog::sym::FuncKind::CheckGtFloat:
  case prog::sym::FuncKind::CheckGtEqFloat:
  case prog::sym::FuncKind::ConvFloatInt:
  case prog::sym::FuncKind::ConvFloatString:
  case prog::sym::FuncKind::ConvFloatChar:
  case prog::sym::FuncKind::ConvFloatLong:

  // Long
  case prog::sym::FuncKind::AddLong:
  case prog::sym::FuncKind::SubLong:
  case prog::sym::FuncKind::MulLong:
  case prog::sym::FuncKind::DivLong:
  case prog::sym::FuncKind::RemLong:
  case prog::sym::FuncKind::NegateLong:
  case prog::sym::FuncKind::ShiftLeftLong:
  case prog::sym::FuncKind::ShiftRightLong:
  case prog::sym::FuncKind::AndLong:
  case prog::sym::FuncKind::OrLong:
  case prog::sym::FuncKind::XorLong:
  case prog::sym::FuncKind::InvLong:
  case prog::sym::FuncKind::CheckEqLong:
  case prog::sym::FuncKind::CheckLeLong:
  case prog::sym::FuncKind::CheckLeEqLong:
  case prog::sym::FuncKind::CheckGtLong:
  case prog::sym::FuncKind::CheckGtEqLong:
  case prog::sym::FuncKind::ConvLongInt:
  case prog::sym::FuncKind::ConvLongFloat:
  case prog::sym::FuncKind::ConvLongString:
  case prog::sym::FuncKind::ConvLongChar:

  // Char
  case prog::sym::FuncKind::ConvCharString:

  // String
  case prog::sym::FuncKind::AddString:
  case prog::sym::FuncKind::LengthString:
  case prog::sym::FuncKind::IndexString:
  case prog::sym::FuncKind::SliceString:
  case prog::sym::FuncKind::AppendChar:
  case prog::sym::FuncKind::CheckEqString:

  // Source location
  // NOTE: These are only present if the frontend was unable to replace it with valid literals.
  case prog::sym::FuncKind::SourceLocFile:
  case prog::sym::FuncKind::SourceLocLine:
  case prog::sym::FuncKind::SourceLocColumn:

    return true;

  default:
    return false;
  }
}

auto precomputeIntrinsic(
    const prog::Program& prog,
    prog::sym::FuncKind funcKind,
    const std::vector<prog::expr::NodePtr>& args) -> prog::expr::NodePtr {

  switch (funcKind) {

  // Int
  case prog::sym::FuncKind::AddInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) + getInt(*args[1]));
  }
  case prog::sym::FuncKind::SubInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) - getInt(*args[1]));
  }
  case prog::sym::FuncKind::MulInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) * getInt(*args[1]));
  }
  case prog::sym::FuncKind::DivInt: {
    assert(args.size() == 2);
    const int32_t lhs = getInt(*args[0]);
    const int32_t rhs = getInt(*args[1]);
    if (rhs == 0) {
      return nullptr;
    }
    return prog::expr::litIntNode(prog, lhs / rhs);
  }
  case prog::sym::FuncKind::RemInt: {
    assert(args.size() == 2);
    const int32_t lhs = getInt(*args[0]);
    const int32_t rhs = getInt(*args[1]);
    if (rhs == 0) {
      return nullptr;
    }
    return prog::expr::litIntNode(prog, lhs % rhs);
  }
  case prog::sym::FuncKind::NegateInt: {
    assert(args.size() == 1);
    return prog::expr::litIntNode(prog, -getInt(*args[0]));
  }
  case prog::sym::FuncKind::ShiftLeftInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) << getInt(*args[1]));
  }
  case prog::sym::FuncKind::ShiftRightInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) >> getInt(*args[1]));
  }
  case prog::sym::FuncKind::AndInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) & getInt(*args[1]));
  }
  case prog::sym::FuncKind::OrInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) | getInt(*args[1]));
  }
  case prog::sym::FuncKind::XorInt: {
    assert(args.size() == 2);
    return prog::expr::litIntNode(prog, getInt(*args[0]) ^ getInt(*args[1]));
  }
  case prog::sym::FuncKind::InvInt: {
    assert(args.size() == 1);
    return prog::expr::litIntNode(prog, ~getInt(*args[0]));
  }
  case prog::sym::FuncKind::CheckEqInt: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getInt(*args[0]) == getInt(*args[1]));
  }
  case prog::sym::FuncKind::CheckLeInt: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getInt(*args[0]) < getInt(*args[1]));
  }
  case prog::sym::FuncKind::CheckLeEqInt: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getInt(*args[0]) <= getInt(*args[1]));
  }
  case prog::sym::FuncKind::CheckGtInt: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getInt(*args[0]) > getInt(*args[1]));
  }
  case prog::sym::FuncKind::CheckGtEqInt: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getInt(*args[0]) >= getInt(*args[1]));
  }
  case prog::sym::FuncKind::CheckIntZero: {
    assert(args.size() == 1);
    return prog::expr::litBoolNode(prog, getInt(*args[0]) == 0);
  }
  case prog::sym::FuncKind::ConvIntLong:
  case prog::sym::FuncKind::ConvCharLong: {
    assert(args.size() == 1);
    return prog::expr::litLongNode(prog, static_cast<int64_t>(getInt(*args[0])));
  }
  case prog::sym::FuncKind::ConvIntFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, static_cast<float>(getInt(*args[0])));
  }
  case prog::sym::FuncKind::ConvIntString: {
    assert(args.size() == 1);
    auto ss = std::stringstream{};
    ss << getInt(*args[0]);
    return prog::expr::litStringNode(prog, ss.str());
  }
  case prog::sym::FuncKind::ConvIntChar: {
    assert(args.size() == 1);
    return prog::expr::litCharNode(prog, static_cast<uint8_t>(getInt(*args[0])));
  }

  // Float
  case prog::sym::FuncKind::AddFloat: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, getFloat(*args[0]) + getFloat(*args[1]));
  }
  case prog::sym::FuncKind::SubFloat: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, getFloat(*args[0]) - getFloat(*args[1]));
  }
  case prog::sym::FuncKind::MulFloat: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, getFloat(*args[0]) * getFloat(*args[1]));
  }
  case prog::sym::FuncKind::DivFloat: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, getFloat(*args[0]) / getFloat(*args[1]));
  }
  case prog::sym::FuncKind::ModFloat: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, fmodf(getFloat(*args[0]), getFloat(*args[1])));
  }
  case prog::sym::FuncKind::PowFloat: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, powf(getFloat(*args[0]), getFloat(*args[1])));
  }
  case prog::sym::FuncKind::SqrtFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, sqrtf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::SinFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, sinf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::CosFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, cosf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::TanFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, tanf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::ASinFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, asinf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::ACosFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, acosf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::ATanFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, atanf(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::ATan2Float: {
    assert(args.size() == 2);
    return prog::expr::litFloatNode(prog, atan2f(getFloat(*args[0]), getFloat(*args[1])));
  }
  case prog::sym::FuncKind::NegateFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, -getFloat(*args[0]));
  }
  case prog::sym::FuncKind::CheckEqFloat: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getFloat(*args[0]) == getFloat(*args[1]));
  }
  case prog::sym::FuncKind::CheckLeFloat: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getFloat(*args[0]) < getFloat(*args[1]));
  }
  case prog::sym::FuncKind::CheckLeEqFloat: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getFloat(*args[0]) <= getFloat(*args[1]));
  }
  case prog::sym::FuncKind::CheckGtFloat: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getFloat(*args[0]) > getFloat(*args[1]));
  }
  case prog::sym::FuncKind::CheckGtEqFloat: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getFloat(*args[0]) >= getFloat(*args[1]));
  }
  case prog::sym::FuncKind::ConvFloatInt: {
    assert(args.size() == 1);
    return prog::expr::litIntNode(prog, static_cast<int32_t>(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::ConvFloatString: {
    assert(args.size() == 1);
    auto f = getFloat(*args[0]);
    if (std::isnan(f)) {
      return prog::expr::litStringNode(prog, "nan");
    }
    auto ss = std::stringstream{};
    ss << std::setprecision(6) << f;
    return prog::expr::litStringNode(prog, ss.str());
  }
  case prog::sym::FuncKind::ConvFloatChar: {
    assert(args.size() == 1);
    return prog::expr::litCharNode(prog, static_cast<uint8_t>(getFloat(*args[0])));
  }
  case prog::sym::FuncKind::ConvFloatLong: {
    assert(args.size() == 1);
    return prog::expr::litLongNode(prog, static_cast<int64_t>(getFloat(*args[0])));
  }

  // Long
  case prog::sym::FuncKind::AddLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) + getLong(*args[1]));
  }
  case prog::sym::FuncKind::SubLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) - getLong(*args[1]));
  }
  case prog::sym::FuncKind::MulLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) * getLong(*args[1]));
  }
  case prog::sym::FuncKind::DivLong: {
    assert(args.size() == 2);
    const int64_t lhs = getLong(*args[0]);
    const int64_t rhs = getLong(*args[1]);
    if (rhs == 0) {
      return nullptr;
    }
    return prog::expr::litLongNode(prog, lhs / rhs);
  }
  case prog::sym::FuncKind::RemLong: {
    assert(args.size() == 2);
    const int64_t lhs = getLong(*args[0]);
    const int64_t rhs = getLong(*args[1]);
    if (rhs == 0) {
      return nullptr;
    }
    return prog::expr::litLongNode(prog, lhs % rhs);
  }
  case prog::sym::FuncKind::NegateLong: {
    assert(args.size() == 1);
    return prog::expr::litLongNode(prog, -getLong(*args[0]));
  }
  case prog::sym::FuncKind::ShiftLeftLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) << getInt(*args[1]));
  }
  case prog::sym::FuncKind::ShiftRightLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) >> getInt(*args[1]));
  }
  case prog::sym::FuncKind::AndLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) & getLong(*args[1]));
  }
  case prog::sym::FuncKind::OrLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) | getLong(*args[1]));
  }
  case prog::sym::FuncKind::XorLong: {
    assert(args.size() == 2);
    return prog::expr::litLongNode(prog, getLong(*args[0]) ^ getLong(*args[1]));
  }
  case prog::sym::FuncKind::InvLong: {
    assert(args.size() == 1);
    return prog::expr::litLongNode(prog, ~getLong(*args[0]));
  }
  case prog::sym::FuncKind::CheckEqLong: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getLong(*args[0]) == getLong(*args[1]));
  }
  case prog::sym::FuncKind::CheckLeLong: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getLong(*args[0]) < getLong(*args[1]));
  }
  case prog::sym::FuncKind::CheckLeEqLong: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getLong(*args[0]) <= getLong(*args[1]));
  }
  case prog::sym::FuncKind::CheckGtLong: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getLong(*args[0]) > getLong(*args[1]));
  }
  case prog::sym::FuncKind::CheckGtEqLong: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getLong(*args[0]) >= getLong(*args[1]));
  }
  case prog::sym::FuncKind::ConvLongInt: {
    assert(args.size() == 1);
    return prog::expr::litIntNode(prog, static_cast<int32_t>(getLong(*args[0])));
  }
  case prog::sym::FuncKind::ConvLongFloat: {
    assert(args.size() == 1);
    return prog::expr::litFloatNode(prog, static_cast<float>(getLong(*args[0])));
  }
  case prog::sym::FuncKind::ConvLongString: {
    assert(args.size() == 1);
    auto ss = std::stringstream{};
    ss << getLong(*args[0]);
    return prog::expr::litStringNode(prog, ss.str());
  }
  case prog::sym::FuncKind::ConvLongChar: {
    assert(args.size() == 1);
    return prog::expr::litCharNode(prog, static_cast<uint8_t>(getLong(*args[0])));
  }

  // Char
  case prog::sym::FuncKind::ConvCharString: {
    assert(args.size() == 1);
    auto c = getChar(*args[0]);
    return prog::expr::litStringNode(prog, std::string(1, c));
  }

  // String
  case prog::sym::FuncKind::AddString: {
    assert(args.size() == 2);
    return prog::expr::litStringNode(prog, getString(*args[0]) + getString(*args[1]));
  }
  case prog::sym::FuncKind::LengthString: {
    assert(args.size() == 1);
    return prog::expr::litIntNode(prog, static_cast<int32_t>(getString(*args[0]).size()));
  }
  case prog::sym::FuncKind::IndexString: {
    assert(args.size() == 2);
    auto str = getString(*args[0]);
    auto idx = getInt(*args[1]);
    if (idx < 0 || static_cast<unsigned>(idx) >= str.length()) {
      return prog::expr::litCharNode(prog, 0);
    }
    return prog::expr::litCharNode(prog, str[idx]);
  }
  case prog::sym::FuncKind::SliceString: {
    assert(args.size() == 3);
    auto str   = getString(*args[0]);
    auto start = getInt(*args[1]);
    auto end   = getInt(*args[2]);
    if (start < 0) {
      start = 0;
    }
    if (end < 0) {
      end = 0;
    }
    if (static_cast<unsigned>(end) >= str.length()) {
      end = str.length();
    }
    if (start > end) {
      start = end;
    }
    return prog::expr::litStringNode(prog, str.substr(start, end - start));
  }
  case prog::sym::FuncKind::AppendChar: {
    assert(args.size() == 2);
    auto result = getString(*args[0]);
    result += getChar(*args[1]);
    return prog::expr::litStringNode(prog, std::move(result));
  }
  case prog::sym::FuncKind::CheckEqString: {
    assert(args.size() == 2);
    return prog::expr::litBoolNode(prog, getString(*args[0]) == getString(*args[1]));
  }

  case prog::sym::FuncKind::SourceLocFile: {
    assert(args.size() == 0);
    return prog::expr::litStringNode(prog, "unknown");
  }
  case prog::sym::FuncKind::SourceLocLine:
  case prog::sym::FuncKind::SourceLocColumn: {
    assert(args.size() == 0);
    return prog::expr::litIntNode(prog, -1);
  }

  default:
    throw std::logic_error{"Unsupported func-kind"};
  }
}

auto maybeSimplifyIntrinsic(
    const prog::Program& prog,
    prog::sym::FuncKind funcKind,
    const std::vector<prog::expr::NodePtr>& args) -> prog::expr::NodePtr {

  switch (funcKind) {
  case prog::sym::FuncKind::CheckEqInt:
    assert(args.size() == 2);
    if (isLiteral(*args[1]) && getInt(*args[1]) == 0) {
      return prog::expr::callExprNode(
          prog, *prog.lookupIntrinsic("int_eq_zero", {prog.getInt()}), cloneToVec(args[0]));
    }
    if (isLiteral(*args[0]) && getInt(*args[0]) == 0) {
      return prog::expr::callExprNode(
          prog, *prog.lookupIntrinsic("int_eq_zero", {prog.getInt()}), cloneToVec(args[1]));
    }
    return nullptr;
  case prog::sym::FuncKind::AddString:
    assert(args.size() == 2);
    if (isLiteral(*args[1]) && getString(*args[1]) == "") {
      return args[0]->clone(nullptr);
    }
    if (isLiteral(*args[0]) && getString(*args[0]) == "") {
      return args[1]->clone(nullptr);
    }
    return nullptr;
  default:
    return nullptr;
  }
}

auto maybePrecomputeReinterpretConv(
    const prog::Program& prog, const prog::expr::Node& arg, prog::sym::TypeId dstType)
    -> prog::expr::NodePtr {

  assert(isLiteral(arg));
  auto& srcTypeDecl = prog.getTypeDecl(arg.getType());
  auto& dstTypeDecl = prog.getTypeDecl(dstType);

  // Char as int.
  if (srcTypeDecl.getKind() == prog::sym::TypeKind::Char &&
      dstTypeDecl.getKind() == prog::sym::TypeKind::Int) {
    return prog::expr::litIntNode(prog, static_cast<int32_t>(getChar(arg)));
  }

  // Bool as int.
  if (srcTypeDecl.getKind() == prog::sym::TypeKind::Bool &&
      dstTypeDecl.getKind() == prog::sym::TypeKind::Int) {
    return prog::expr::litIntNode(prog, getBool(arg) ? 1 : 0);
  }

  // Int as float.
  if (srcTypeDecl.getKind() == prog::sym::TypeKind::Int &&
      dstTypeDecl.getKind() == prog::sym::TypeKind::Float) {
    auto src = getInt(arg);
    return prog::expr::litFloatNode(prog, reinterpret_cast<float&>(src));
  }

  // Float as int.
  if (srcTypeDecl.getKind() == prog::sym::TypeKind::Float &&
      dstTypeDecl.getKind() == prog::sym::TypeKind::Int) {
    auto src = getFloat(arg);
    return prog::expr::litIntNode(prog, reinterpret_cast<int&>(src));
  }

  // Enum to int.
  if (srcTypeDecl.getKind() == prog::sym::TypeKind::Enum &&
      dstTypeDecl.getKind() == prog::sym::TypeKind::Int) {
    const auto val = arg.downcast<prog::expr::LitEnumNode>()->getVal();
    return prog::expr::litIntNode(prog, val);
  }

  // Int to enum.
  if (srcTypeDecl.getKind() == prog::sym::TypeKind::Int &&
      dstTypeDecl.getKind() == prog::sym::TypeKind::Enum) {
    auto val = getInt(arg);
    return prog::expr::litEnumNode(prog, dstTypeDecl.getId(), val);
  }

  return nullptr;
}

} // namespace opt::internal
