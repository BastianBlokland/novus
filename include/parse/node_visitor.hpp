#pragma once

namespace parse {

class CommentNode;
class ErrorNode;
class AnonFuncExprNode;
class BinaryExprNode;
class CallExprNode;
class ConditionalExprNode;
class ConstDeclExprNode;
class IdExprNode;
class FieldExprNode;
class GroupExprNode;
class IndexExprNode;
class IntrinsicExprNode;
class IsExprNode;
class LitExprNode;
class ParenExprNode;
class SwitchExprElseNode;
class SwitchExprIfNode;
class SwitchExprNode;
class UnaryExprNode;
class EnumDeclStmtNode;
class ExecStmtNode;
class FuncDeclStmtNode;
class ImportStmtNode;
class StructDeclStmtNode;
class UnionDeclStmtNode;

class NodeVisitor {
public:
  virtual auto visit(const CommentNode& n) -> void         = 0;
  virtual auto visit(const ErrorNode& n) -> void           = 0;
  virtual auto visit(const AnonFuncExprNode& n) -> void    = 0;
  virtual auto visit(const BinaryExprNode& n) -> void      = 0;
  virtual auto visit(const CallExprNode& n) -> void        = 0;
  virtual auto visit(const ConditionalExprNode& n) -> void = 0;
  virtual auto visit(const ConstDeclExprNode& n) -> void   = 0;
  virtual auto visit(const IdExprNode& n) -> void          = 0;
  virtual auto visit(const FieldExprNode& n) -> void       = 0;
  virtual auto visit(const GroupExprNode& n) -> void       = 0;
  virtual auto visit(const IndexExprNode& n) -> void       = 0;
  virtual auto visit(const IntrinsicExprNode& n) -> void   = 0;
  virtual auto visit(const IsExprNode& n) -> void          = 0;
  virtual auto visit(const LitExprNode& n) -> void         = 0;
  virtual auto visit(const ParenExprNode& n) -> void       = 0;
  virtual auto visit(const SwitchExprElseNode& n) -> void  = 0;
  virtual auto visit(const SwitchExprIfNode& n) -> void    = 0;
  virtual auto visit(const SwitchExprNode& n) -> void      = 0;
  virtual auto visit(const UnaryExprNode& n) -> void       = 0;
  virtual auto visit(const EnumDeclStmtNode& n) -> void    = 0;
  virtual auto visit(const ExecStmtNode& n) -> void        = 0;
  virtual auto visit(const FuncDeclStmtNode& n) -> void    = 0;
  virtual auto visit(const ImportStmtNode& n) -> void      = 0;
  virtual auto visit(const StructDeclStmtNode& n) -> void  = 0;
  virtual auto visit(const UnionDeclStmtNode& n) -> void   = 0;
};

} // namespace parse
