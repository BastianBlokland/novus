#pragma once
#include "prog/sym/func_id.hpp"
#include <functional>

namespace prog::sym {

// Generate a hash for a FuncId.
class FuncIdHasher final {
public:
  auto operator()(const FuncId& id) const -> std::size_t {
    return std::hash<unsigned int>{}(id.m_id);
  }
};

} // namespace prog::sym
