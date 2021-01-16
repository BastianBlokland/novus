#pragma once
#include "prog/program.hpp"
#include <vector>

namespace opt::internal {

[[nodiscard]] auto isPrecomputableIntrinsic(prog::sym::FuncKind funcKind) -> bool;

[[nodiscard]] auto precomputeIntrinsic(
    const prog::Program& prog,
    prog::sym::FuncKind funcKind,
    const std::vector<prog::expr::NodePtr>& args) -> prog::expr::NodePtr;

// NOTE: Returns nullptr if it was not possible to precompute the reinterpret conversion.
[[nodiscard]] auto maybePrecomputeReinterpretConv(
    const prog::Program& prog, const prog::expr::Node& arg, prog::sym::TypeId dstType)
    -> prog::expr::NodePtr;

} // namespace opt::internal
