#pragma once

namespace prog::expr {

class AssignExprNode;
class SwitchExprNode;
class CallExprNode;
class ConstExprNode;
class FieldExprNode;
class GroupExprNode;
class LitBoolNode;
class LitIntNode;
class LitStringNode;

class NodeVisitor {
public:
  virtual auto visit(const AssignExprNode& n) -> void = 0;
  virtual auto visit(const SwitchExprNode& n) -> void = 0;
  virtual auto visit(const CallExprNode& n) -> void   = 0;
  virtual auto visit(const ConstExprNode& n) -> void  = 0;
  virtual auto visit(const FieldExprNode& n) -> void  = 0;
  virtual auto visit(const GroupExprNode& n) -> void  = 0;
  virtual auto visit(const LitBoolNode& n) -> void    = 0;
  virtual auto visit(const LitIntNode& n) -> void     = 0;
  virtual auto visit(const LitStringNode& n) -> void  = 0;
};

} // namespace prog::expr
