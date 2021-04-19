#pragma once
#include "novasm/executable.hpp"
#include <optional>

namespace novasm {

// Version number for the binary representation of the novus assembly format.
// Increase this when performing breaking changes to the format.
// TODO(bastian): Add system for defining migrations.
const uint16_t executableFormatVersion = 17U;

// Write a binary representation of the executable file to the output iterator.
template <typename OutputItr>
auto serialize(const Executable& executable, OutputItr outItr) noexcept -> OutputItr;

// Read an executable file in its binary from from the input iterator.
template <typename InputItrBegin, typename InputEndItr>
auto deserialize(InputItrBegin begin, InputEndItr end) noexcept -> std::optional<Executable>;

} // namespace novasm
