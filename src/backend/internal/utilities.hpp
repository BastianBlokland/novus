#pragma once
#include "prog/sym/const_id.hpp"
#include "prog/sym/func_id.hpp"
#include <string>

namespace backend::internal {

auto getLabel(prog::sym::FuncId funcId) -> std::string;

auto getConstId(prog::sym::ConstId constId) -> uint8_t;

} // namespace backend::internal
