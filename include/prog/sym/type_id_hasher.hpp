#pragma once
#include "prog/sym/type_id.hpp"

namespace prog::sym {

// Generate a hash for a TypeId.
class TypeIdHasher final {
public:
  auto operator()(const TypeId& id) const -> std::size_t;
};

} // namespace prog::sym
