#pragma once
#include "prog/sym/const_id.hpp"
#include <functional>

namespace prog::sym {

// Generate a hash for a ConstId.
class ConstIdHasher final {
public:
  auto operator()(const ConstId& id) const -> std::size_t {
    return std::hash<unsigned int>{}(id.m_id);
  }
};

} // namespace prog::sym
