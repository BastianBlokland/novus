#pragma once
#include <iostream>

namespace frontend {

enum class DiagSeverity { Warning, Error };

auto operator<<(std::ostream& out, const DiagSeverity& rhs) -> std::ostream&;

} // namespace frontend
