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
  case NodeType::ExpUnaryOp:
    out << "exp-unary-op";
    break;
  case NodeType::ExpBinaryOp:
    out << "exp-binary-op";
    break;
  case NodeType::ExpSwitch:
    out << "exp-switch";
    break;
  case NodeType::ExpLit:
    out << "exp-lit";
    break;
  case NodeType::ExpConst:
    out << "exp-const";
    break;
  case NodeType::ExpConstDecl:
    out << "exp-const-decl";
    break;
  case NodeType::ExpCall:
    out << "exp-call";
    break;
  case NodeType::ExpComma:
    out << "exp-comma";
    break;
  case NodeType::ExpParan:
    out << "exp-paran";
    break;
  case NodeType::Error:
    out << "error";
    break;
  }
  return out;
}

} // namespace parse
