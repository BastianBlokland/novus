#pragma once
#include "prog/sym/func_id.hpp"

namespace prog::sym {

class FuncIdHasher final {
public:
  auto operator()(const FuncId& id) const -> std::size_t;
};

} // namespace prog::sym
