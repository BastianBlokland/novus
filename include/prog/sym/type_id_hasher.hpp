#pragma once
#include "prog/sym/type_id.hpp"
#include <functional>

namespace prog::sym {

// Generate a hash for a TypeId.
class TypeIdHasher final {
public:
  auto operator()(const TypeId& id) const -> std::size_t {
    return std::hash<unsigned int>{}(id.m_id);
  }
};

} // namespace prog::sym
