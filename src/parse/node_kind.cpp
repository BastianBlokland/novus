#include "parse/node_kind.hpp"

namespace parse {

auto operator<<(std::ostream& out, const NodeKind& rhs) -> std::ostream& {
  switch (rhs) {
  case NodeKind::StmtFuncDecl:
    out << "stmt-func-decl";
    break;
  case NodeKind::StmtPrint:
    out << "stmt-print";
    break;
  case NodeKind::ExprUnaryOp:
    out << "expr-unary-op";
    break;
  case NodeKind::ExprBinaryOp:
    out << "expr-binary-op";
    break;
  case NodeKind::ExprSwitch:
    out << "expr-switch";
    break;
  case NodeKind::ExprSwitchIf:
    out << "expr-switch-if";
    break;
  case NodeKind::ExprSwitchElse:
    out << "expr-switch-else";
    break;
  case NodeKind::ExprLit:
    out << "expr-lit";
    break;
  case NodeKind::ExprConst:
    out << "expr-const";
    break;
  case NodeKind::ExprConstDecl:
    out << "expr-const-decl";
    break;
  case NodeKind::ExprCall:
    out << "expr-call";
    break;
  case NodeKind::ExprGroup:
    out << "expr-group";
    break;
  case NodeKind::ExprParen:
    out << "expr-paren";
    break;
  case NodeKind::Error:
    out << "error";
    break;
  }
  return out;
}

} // namespace parse
