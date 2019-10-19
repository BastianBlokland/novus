#include "parse/node_type.hpp"

namespace parse {

auto operator<<(std::ostream& out, const NodeType& rhs) -> std::ostream& {
  switch (rhs) {
  case NodeType::StmtFunDecl:
    out << "stmt-fun-decl";
    break;
  case NodeType::StmtPrint:
    out << "stmt-print";
    break;
  case NodeType::ExprUnaryOp:
    out << "expr-unary-op";
    break;
  case NodeType::ExprBinaryOp:
    out << "expr-binary-op";
    break;
  case NodeType::ExprSwitch:
    out << "expr-switch";
    break;
  case NodeType::ExprSwitchIf:
    out << "expr-switch-if";
    break;
  case NodeType::ExprLit:
    out << "expr-lit";
    break;
  case NodeType::ExprConst:
    out << "expr-const";
    break;
  case NodeType::ExprConstDecl:
    out << "expr-const-decl";
    break;
  case NodeType::ExprCall:
    out << "expr-call";
    break;
  case NodeType::ExprGroup:
    out << "expr-group";
    break;
  case NodeType::ExprParen:
    out << "expr-paren";
    break;
  case NodeType::Error:
    out << "error";
    break;
  }
  return out;
}

} // namespace parse
