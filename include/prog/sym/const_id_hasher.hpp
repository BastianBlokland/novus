#pragma once
#include "prog/sym/const_id.hpp"

namespace prog::sym {

class ConstIdHasher final {
public:
  auto operator()(const ConstId& id) const -> std::size_t;
};

} // namespace prog::sym
