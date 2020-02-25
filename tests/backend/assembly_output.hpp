#pragma once
#include "novasm/assembly.hpp"
#include <iostream>

namespace novasm {

auto operator<<(std::ostream& out, const novasm::Assembly& rhs) -> std::ostream&;

} // namespace novasm
