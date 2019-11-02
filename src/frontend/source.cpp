#include "frontend/source.hpp"
#include <utility>

namespace frontend {

Source::Source(std::string id, std::vector<parse::NodePtr> nodes, input::Info info) :
    m_id{std::move(id)}, m_nodes{std::move(nodes)}, m_info{std::move(info)} {};

auto Source::begin() const noexcept -> iterator { return m_nodes.begin(); }

auto Source::end() const noexcept -> iterator { return m_nodes.end(); }

auto Source::getId() const noexcept -> std::string { return m_id; }

auto Source::getTextPos(unsigned int pos) const noexcept -> input::TextPos {
  return m_info.getTextPos(pos);
}

auto Source::accept(parse::NodeVisitor* visitor) const -> void {
  for (const auto& n : m_nodes) {
    n->accept(visitor);
  }
}

auto operator<<(std::ostream& out, const Source& rhs) -> std::ostream& { return out << rhs.m_id; }

} // namespace frontend
