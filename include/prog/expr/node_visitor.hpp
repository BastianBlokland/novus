#pragma once

namespace prog::expr {

class AssignExprNode;
class SwitchExprNode;
class CallExprNode;
class CallDynExprNode;
class CallSelfExprNode;
class ClosureNode;
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
class LitLongNode;
class LitStringNode;
class LitCharNode;
class LitEnumNode;

class NodeVisitor {
public:
  virtual auto visit(const AssignExprNode& n) -> void     = 0;
  virtual auto visit(const SwitchExprNode& n) -> void     = 0;
  virtual auto visit(const CallExprNode& n) -> void       = 0;
  virtual auto visit(const CallDynExprNode& n) -> void    = 0;
  virtual auto visit(const CallSelfExprNode& n) -> void   = 0;
  virtual auto visit(const ClosureNode& n) -> void        = 0;
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
  virtual auto visit(const LitLongNode& n) -> void        = 0;
  virtual auto visit(const LitStringNode& n) -> void      = 0;
  virtual auto visit(const LitCharNode& n) -> void        = 0;
  virtual auto visit(const LitEnumNode& n) -> void        = 0;
};

} // namespace prog::expr
