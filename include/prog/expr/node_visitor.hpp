#pragma once

namespace prog::expr {

class AssignExprNode;
class BranchExprNode;
class CallExprNode;
class ConstExprNode;
class GroupExprNode;
class LitBoolNode;
class LitIntNode;

class NodeVisitor {
public:
  virtual auto visit(const AssignExprNode& n) -> void = 0;
  virtual auto visit(const BranchExprNode& n) -> void = 0;
  virtual auto visit(const CallExprNode& n) -> void   = 0;
  virtual auto visit(const ConstExprNode& n) -> void  = 0;
  virtual auto visit(const GroupExprNode& n) -> void  = 0;
  virtual auto visit(const LitBoolNode& n) -> void    = 0;
  virtual auto visit(const LitIntNode& n) -> void     = 0;
};

} // namespace prog::expr
