#pragma once
#include "prog/expr/node_visitor_deep.hpp"
#include "prog/sym/const_id_hasher.hpp"
#include <unordered_map>

namespace opt::internal {

class FindUsedConsts final : public prog::expr::DeepNodeVisitor {
  using UsageMap =
      typename std::unordered_map<prog::sym::ConstId, unsigned int, prog::sym::ConstIdHasher>;

public:
  explicit FindUsedConsts(UsageMap* usage);

  auto visit(const prog::expr::ConstExprNode& n) -> void override;

private:
  UsageMap* m_usage;
};

} // namespace opt::internal
