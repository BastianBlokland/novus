#pragma once
#include "prog/sym/source_id.hpp"
#include <functional>

namespace prog::sym {

// Generate a hash for a SourceId.
class SourceIdHasher final {
public:
  auto operator()(const SourceId& id) const -> std::size_t {
    return std::hash<unsigned int>{}(id.m_id);
  }
};

} // namespace prog::sym
