#pragma once

namespace vm::internal {

enum class RefKind : unsigned int {
  String = 1,
  Struct = 2,
};

} // namespace vm::internal
