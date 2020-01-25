#pragma once
#include "internal/type_info.hpp"

namespace frontend::internal {

auto validateType(prog::sym::TypeId id, const TypeInfo& info) -> void;

} // namespace frontend::internal
