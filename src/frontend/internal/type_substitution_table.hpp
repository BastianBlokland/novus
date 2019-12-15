#pragma once
#include "prog/sym/type_id.hpp"
#include <unordered_map>
#include <utility>

namespace frontend::internal {

class TypeSubstitutionTable final {
public:
  TypeSubstitutionTable()                                     = default;
  TypeSubstitutionTable(const TypeSubstitutionTable& rhs)     = delete;
  TypeSubstitutionTable(TypeSubstitutionTable&& rhs) noexcept = default;
  ~TypeSubstitutionTable()                                    = default;

  auto operator=(const TypeSubstitutionTable& rhs) -> TypeSubstitutionTable& = delete;
  auto operator=(TypeSubstitutionTable&& rhs) noexcept -> TypeSubstitutionTable& = delete;

  [[nodiscard]] auto lookupType(const std::string& name) const -> std::optional<prog::sym::TypeId>;

  auto declare(std::string name, prog::sym::TypeId type) -> void;

private:
  std::unordered_map<std::string, prog::sym::TypeId> m_subs;
};

} // namespace frontend::internal
