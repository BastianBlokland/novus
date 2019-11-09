#include "prog/sym/func_id_hasher.hpp"
#include <functional>

namespace prog::sym {

auto FuncIdHasher::operator()(const FuncId& id) const -> std::size_t {
  return std::hash<unsigned int>{}(id.m_id);
}

} // namespace prog::sym
