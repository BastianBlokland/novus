#include "utilities.hpp"
#include <limits>
#include <sstream>

namespace backend::internal {

auto getLabel(prog::sym::FuncId funcId) -> std::string {
  std::ostringstream oss;
  oss << funcId;
  return oss.str();
}

auto getConstId(prog::sym::ConstId constId) -> uint8_t {
  auto constNum = constId.getNum();
  if (constNum > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 constants in one scope are not supported"};
  }
  return static_cast<uint8_t>(constNum);
}

} // namespace backend::internal
