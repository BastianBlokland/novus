#pragma once

namespace prog::sym {

enum class OverloadFlags : unsigned int {
  None             = 0U,
  ExclPureFuncs    = 1U << 0U, // Exclude pure functions (non actions).
  ExclActions      = 1U << 1U, // Exclude non-pure functions (actions).
  ExclNonUser      = 1U << 2U, // Exclude build-in functions.
  NoConvOnFirstArg = 1U << 3U, // Disallow implicit conversions on the first argument.
  NoOptArgs        = 1U << 4U, // Disallow optional arguments (requires passing all args).
};

inline auto operator|(OverloadFlags lhs, OverloadFlags rhs) noexcept {
  return static_cast<OverloadFlags>(
      static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline auto operator&(OverloadFlags lhs, OverloadFlags rhs) noexcept {
  return static_cast<OverloadFlags>(
      static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

// Options to use during overload resolution.
class OverloadOptions final {
public:
  OverloadOptions() : m_flags{OverloadFlags::None}, m_maxImplicitConvs{-1} {}

  explicit OverloadOptions(OverloadFlags flags) : m_flags{flags}, m_maxImplicitConvs{-1} {}

  explicit OverloadOptions(int maxImplicitConvs) :
      m_flags{OverloadFlags::None}, m_maxImplicitConvs{maxImplicitConvs} {}

  OverloadOptions(OverloadFlags flags, int maxImplicitConvs) :
      m_flags{flags}, m_maxImplicitConvs{maxImplicitConvs} {}

  template <OverloadFlags Flag>
  [[nodiscard]] inline auto hasFlag() const noexcept {
    return (m_flags & Flag) == Flag;
  }
  [[nodiscard]] inline auto getFlags() const noexcept { return m_flags; }
  [[nodiscard]] inline auto getMaxImplicitConvs() const noexcept { return m_maxImplicitConvs; }

private:
  OverloadFlags m_flags;
  int m_maxImplicitConvs;
};

} // namespace prog::sym
