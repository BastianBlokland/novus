#pragma once
#include "novasm/assembly.hpp"
#include <optional>

namespace novasm {

const uint16_t assemblyFormatVersion = 1U;

template <typename OutputItr>
auto serialize(const Assembly& assembly, OutputItr outItr) noexcept -> OutputItr;

template <typename InputItrBegin, typename InputEndItr>
auto deserialize(InputItrBegin begin, InputEndItr end) noexcept -> std::optional<Assembly>;

} // namespace novasm
