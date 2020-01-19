#pragma once

namespace prog::sym {

enum class OverloadFlags : unsigned int {
  None                  = 0U,
  ExclPureFuncs         = 1U << 0U,
  ExclActions           = 1U << 1U,
  DisableConvOnFirstArg = 1U << 2U,
};

inline auto operator|(OverloadFlags lhs, OverloadFlags rhs) noexcept {
  return static_cast<OverloadFlags>(
      static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline auto operator&(OverloadFlags lhs, OverloadFlags rhs) noexcept {
  return static_cast<OverloadFlags>(
      static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

class OverloadOptions final {
public:
  OverloadOptions() : m_flags{OverloadFlags::None}, m_maxConversions{-1} {}

  explicit OverloadOptions(OverloadFlags flags) : m_flags{flags}, m_maxConversions{-1} {}

  explicit OverloadOptions(int maxConversions) :
      m_flags{OverloadFlags::None}, m_maxConversions{maxConversions} {}

  OverloadOptions(OverloadFlags flags, int maxConversions) :
      m_flags{flags}, m_maxConversions{maxConversions} {}

  template <OverloadFlags Flag>
  [[nodiscard]] inline auto hasFlag() const noexcept {
    return (m_flags & Flag) == Flag;
  }
  [[nodiscard]] inline auto getFlags() const noexcept { return m_flags; }
  [[nodiscard]] inline auto getMaxConversions() const noexcept { return m_maxConversions; }

private:
  OverloadFlags m_flags;
  int m_maxConversions;
};

} // namespace prog::sym
