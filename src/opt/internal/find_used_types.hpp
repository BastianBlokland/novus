#pragma once
#include "prog/expr/node_visitor_deep.hpp"
#include "prog/program.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <unordered_set>

namespace opt::internal {

class FindUsedTypes final : public prog::expr::DeepNodeVisitor {

  using TypeSet = typename std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>;

public:
  FindUsedTypes(const prog::Program& prog, TypeSet* types);

  auto markType(prog::sym::TypeId type) -> void;

protected:
  auto visitNode(const prog::expr::Node* n) -> void override;

private:
  const prog::Program& m_prog;
  TypeSet* m_types;
};

} // namespace opt::internal
