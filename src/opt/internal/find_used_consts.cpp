#include "find_used_consts.hpp"
#include "prog/expr/nodes.hpp"

namespace opt::internal {

FindUsedConsts::FindUsedConsts(UsageMap* usage) : m_usage{usage} {
  if (m_usage == nullptr) {
    throw std::invalid_argument{"Usage map cannot be null"};
  }
}

auto FindUsedConsts::visit(const prog::expr::ConstExprNode& n) -> void { ++(*m_usage)[n.getId()]; }

} // namespace opt::internal
