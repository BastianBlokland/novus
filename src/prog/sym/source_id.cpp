#include "prog/sym/source_id.hpp"

namespace prog::sym {

auto SourceId::none() noexcept -> SourceId { return SourceId{0}; };

SourceId::SourceId(unsigned int id) : m_id{id} {}

auto SourceId::operator==(const SourceId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto SourceId::operator!=(const SourceId& rhs) const noexcept -> bool {
  return !SourceId::operator==(rhs);
}

auto SourceId::isSet() const noexcept -> bool { return m_id != 0; }

auto SourceId::getNum() const noexcept -> unsigned int { return m_id; }

auto operator<<(std::ostream& out, const SourceId& rhs) -> std::ostream& {
  return out << "source-" << rhs.m_id;
}

} // namespace prog::sym
