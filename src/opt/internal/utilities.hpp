#pragma once
#include "prog/expr/node.hpp"
#include <string>
#include <vector>

namespace prog::expr {
class Rewriter;
}

namespace opt::internal {

[[nodiscard]] auto getInt(const prog::expr::Node& n) -> int32_t;

[[nodiscard]] auto getBool(const prog::expr::Node& n) -> bool;

[[nodiscard]] auto getFloat(const prog::expr::Node& n) -> float;

[[nodiscard]] auto getLong(const prog::expr::Node& n) -> int64_t;

[[nodiscard]] auto getChar(const prog::expr::Node& n) -> uint8_t;

[[nodiscard]] auto getString(const prog::expr::Node& n) -> std::string;

[[nodiscard]] auto
rewriteAll(const std::vector<prog::expr::NodePtr>& nodes, prog::expr::Rewriter* rewriter)
    -> std::vector<prog::expr::NodePtr>;

inline auto copySourceAttr(prog::expr::Node& to, const prog::expr::Node& from) -> void {
  to.setSourceId(from.getSourceId());
}

} // namespace opt::internal
