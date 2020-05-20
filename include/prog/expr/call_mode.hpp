#pragma once

namespace prog::expr {

// Different ways of how a call can be executed.
enum class CallMode {
  Normal,
  Forked, // Forked calls are executed on a different thread. Return value is a future type.
  Lazy,   // Lazy calls are delayed until the value is requested. Return value is a lazy type.
};

} // namespace prog::expr
