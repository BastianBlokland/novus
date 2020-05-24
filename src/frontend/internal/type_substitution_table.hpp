#pragma once
#include "prog/sym/type_id.hpp"
#include <optional>
#include <unordered_map>
#include <utility>

namespace frontend::internal {

class TypeSubstitutionTable final {
public:
  TypeSubstitutionTable()                                 = default;
  TypeSubstitutionTable(const TypeSubstitutionTable& rhs) = delete;
  TypeSubstitutionTable(TypeSubstitutionTable&& rhs)      = default;
  ~TypeSubstitutionTable()                                = default;

  auto operator=(const TypeSubstitutionTable& rhs) -> TypeSubstitutionTable& = delete;
  auto operator=(TypeSubstitutionTable&& rhs) noexcept -> TypeSubstitutionTable& = delete;

  [[nodiscard]] auto lookupType(const std::string& name) const -> std::optional<prog::sym::TypeId> {
    const auto itr = m_subs.find(name);
    if (itr == m_subs.end()) {
      return std::nullopt;
    }
    return itr->second;
  }

  auto declare(std::string name, prog::sym::TypeId type) -> void {
    if (!type.isConcrete()) {
      throw std::invalid_argument{"Type has to be a concrete type"};
    }
    if (name.empty()) {
      throw std::invalid_argument{"Name has to contain aleast 1 char"};
    }
    if (!m_subs.insert({std::move(name), type}).second) {
      throw std::logic_error{"Substitution with an identical name has already been registered"};
    }
  }

private:
  std::unordered_map<std::string, prog::sym::TypeId> m_subs;
};

} // namespace frontend::internal
