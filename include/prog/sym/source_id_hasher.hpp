#pragma once
#include "prog/sym/source_id.hpp"

namespace prog::sym {

// Generate a hash for a SourceId.
class SourceIdHasher final {
public:
  auto operator()(const SourceId& id) const -> std::size_t;
};

} // namespace prog::sym
