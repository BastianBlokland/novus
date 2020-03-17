#include "prog/sym/const_id_hasher.hpp"
#include <functional>

namespace prog::sym {

auto ConstIdHasher::operator()(const ConstId& id) const -> std::size_t {
  return std::hash<unsigned int>{}(id.m_id);
}

} // namespace prog::sym
