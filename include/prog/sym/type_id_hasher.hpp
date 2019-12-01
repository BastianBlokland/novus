#pragma once
#include "prog/sym/type_id.hpp"

namespace prog::sym {

class TypeIdHasher final {
public:
  auto operator()(const TypeId& id) const -> std::size_t;
};

} // namespace prog::sym
