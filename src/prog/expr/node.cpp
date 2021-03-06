#include "prog/expr/node.hpp"
#include "prog/sym/source_id.hpp"

namespace prog::expr {

Node::Node(NodeKind kind) : m_kind{kind}, m_sourceId(sym::SourceId::none()) {}

auto Node::getKind() const -> NodeKind { return m_kind; }

auto Node::hasSourceId() const -> bool { return m_sourceId.isSet(); }

auto Node::getSourceId() const -> sym::SourceId { return m_sourceId; }

auto Node::setSourceId(sym::SourceId source) -> void { m_sourceId = source; }

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream& {
  return out << rhs.toString();
}

} // namespace prog::expr
