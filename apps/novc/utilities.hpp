#pragma once
#include "filesystem.hpp"

namespace novc {

auto setOutputFilePermissions(filesystem::path filePath) noexcept -> bool;

} // namespace novc
