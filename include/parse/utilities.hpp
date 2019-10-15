#pragma once
#include <algorithm>

namespace parse {

template <typename Container>
auto cloneSet(const Container& cont) -> Container {
  auto result = Container{};
  std::transform(cont.begin(), cont.end(), std::back_inserter(result), [](const auto& n) {
    return n->clone();
  });
  return result;
}

} // namespace parse
