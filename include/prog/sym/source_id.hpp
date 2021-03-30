#pragma once
#include <iostream>

namespace prog::sym {

// Opaque identifier for a region of source-code.
//
// Usefull to attach knowledge about the source location to program expressions. They are opaque
// identifiers so as much information can be attached to program as desired by keeping seperate
// tables that map SourceId's.
//
// Note: 0 is a special value to indicate that no source-id is present.
class SourceId final {
  friend class SourceIdHasher;
  friend auto operator<<(std::ostream& out, const SourceId& rhs) -> std::ostream&;

public:
  [[nodiscard]] static auto none() noexcept -> SourceId { return SourceId{0}; };

  explicit SourceId(unsigned int id) : m_id{id} {}

  auto operator==(const SourceId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const SourceId& rhs) const noexcept -> bool { return !SourceId::operator==(rhs); }

  [[nodiscard]] auto isSet() const noexcept -> bool { return m_id != 0; }
  [[nodiscard]] auto getNum() const noexcept -> unsigned int { return m_id; }

private:
  unsigned int m_id;
};

inline auto operator<<(std::ostream& out, const SourceId& rhs) -> std::ostream& {
  return out << "source-" << rhs.m_id;
}

} // namespace prog::sym
