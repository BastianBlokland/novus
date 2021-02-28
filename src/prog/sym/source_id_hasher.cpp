#include "prog/sym/source_id_hasher.hpp"
#include <functional>

namespace prog::sym {

auto SourceIdHasher::operator()(const SourceId& id) const -> std::size_t {
  return std::hash<unsigned int>{}(id.m_id);
}

} // namespace prog::sym
