#pragma once
#include <iostream>

namespace lex {

enum class Keyword { Function };

std::ostream& operator<<(std::ostream& out, const Keyword& rhs);

} // namespace lex
