#pragma once

namespace prog::expr {

class AssignExprNode;
class SwitchExprNode;
class CallExprNode;
class CallDynExprNode;
class ConstExprNode;
class FieldExprNode;
class GroupExprNode;
class UnionCheckExprNode;
class UnionGetExprNode;
class FailNode;
class LitBoolNode;
class LitFloatNode;
class LitFuncNode;
class LitIntNode;
class LitStringNode;

class NodeVisitor {
public:
  virtual auto visit(const AssignExprNode& n) -> void     = 0;
  virtual auto visit(const SwitchExprNode& n) -> void     = 0;
  virtual auto visit(const CallExprNode& n) -> void       = 0;
  virtual auto visit(const CallDynExprNode& n) -> void    = 0;
  virtual auto visit(const ConstExprNode& n) -> void      = 0;
  virtual auto visit(const FieldExprNode& n) -> void      = 0;
  virtual auto visit(const GroupExprNode& n) -> void      = 0;
  virtual auto visit(const UnionCheckExprNode& n) -> void = 0;
  virtual auto visit(const UnionGetExprNode& n) -> void   = 0;
  virtual auto visit(const FailNode& n) -> void           = 0;
  virtual auto visit(const LitBoolNode& n) -> void        = 0;
  virtual auto visit(const LitFloatNode& n) -> void       = 0;
  virtual auto visit(const LitFuncNode& n) -> void        = 0;
  virtual auto visit(const LitIntNode& n) -> void         = 0;
  virtual auto visit(const LitStringNode& n) -> void      = 0;
};

} // namespace prog::expr
