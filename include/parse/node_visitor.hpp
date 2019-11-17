#pragma once

namespace parse {

class ErrorNode;
class BinaryExprNode;
class CallExprNode;
class ConditionalExprNode;
class ConstDeclExprNode;
class ConstExprNode;
class GroupExprNode;
class LitExprNode;
class ParenExprNode;
class SwitchExprElseNode;
class SwitchExprIfNode;
class SwitchExprNode;
class UnaryExprNode;
class ExecStmtNode;
class FuncDeclStmtNode;

class NodeVisitor {
public:
  virtual auto visit(const ErrorNode& n) -> void           = 0;
  virtual auto visit(const BinaryExprNode& n) -> void      = 0;
  virtual auto visit(const CallExprNode& n) -> void        = 0;
  virtual auto visit(const ConditionalExprNode& n) -> void = 0;
  virtual auto visit(const ConstDeclExprNode& n) -> void   = 0;
  virtual auto visit(const ConstExprNode& n) -> void       = 0;
  virtual auto visit(const GroupExprNode& n) -> void       = 0;
  virtual auto visit(const LitExprNode& n) -> void         = 0;
  virtual auto visit(const ParenExprNode& n) -> void       = 0;
  virtual auto visit(const SwitchExprElseNode& n) -> void  = 0;
  virtual auto visit(const SwitchExprIfNode& n) -> void    = 0;
  virtual auto visit(const SwitchExprNode& n) -> void      = 0;
  virtual auto visit(const UnaryExprNode& n) -> void       = 0;
  virtual auto visit(const ExecStmtNode& n) -> void        = 0;
  virtual auto visit(const FuncDeclStmtNode& n) -> void    = 0;
};

} // namespace parse
