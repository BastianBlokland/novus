#pragma once
#include "novasm/executable.hpp"
#include <iostream>

namespace novasm {

auto operator<<(std::ostream& out, const novasm::Executable& rhs) -> std::ostream&;

} // namespace novasm
