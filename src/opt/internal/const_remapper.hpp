#pragma once
#include "prog/expr/node_assign.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/expr/rewriter.hpp"
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/const_id_hasher.hpp"
#include <unordered_map>

namespace opt::internal {

using ConstRemapTable =
    typename std::unordered_map<prog::sym::ConstId, prog::sym::ConstId, prog::sym::ConstIdHasher>;

class ConstRemapper final : public prog::expr::Rewriter {
public:
  ConstRemapper(
      const prog::Program& prog,
      const prog::sym::ConstDeclTable& consts,
      const ConstRemapTable& remapTable);

  auto rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr override;

  auto hasModified() -> bool override { return m_modified; }

private:
  const prog::Program& m_prog;
  const prog::sym::ConstDeclTable& m_consts;
  const ConstRemapTable& m_remapTable;
  bool m_modified;

  [[nodiscard]] auto remap(prog::sym::ConstId constId) -> prog::sym::ConstId;
};

} // namespace opt::internal
