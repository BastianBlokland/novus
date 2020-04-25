#pragma once
#include "novasm/assembler.hpp"

namespace backend::internal {

auto genLazyGet(novasm::Assembler* asmb) -> void;

} // namespace backend::internal
