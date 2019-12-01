#include "prog/sym/type_id_hasher.hpp"
#include <functional>

namespace prog::sym {

auto TypeIdHasher::operator()(const TypeId& id) const -> std::size_t {
  return std::hash<unsigned int>{}(id.m_id);
}

} // namespace prog::sym
