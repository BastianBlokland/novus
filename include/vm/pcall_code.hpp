#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class PCallCode : uint8_t { Print = 10 };

auto operator<<(std::ostream& out, const PCallCode& rhs) -> std::ostream&;

} // namespace vm
