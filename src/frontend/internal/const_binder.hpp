#pragma once
#include "prog/sym/const_decl_table.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

namespace frontend::internal {

class ConstBinder final {
public:
  ConstBinder() = delete;
  ConstBinder(
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      ConstBinder* parent);

  [[nodiscard]] auto getConsts() -> prog::sym::ConstDeclTable*;

  [[nodiscard]] auto getVisibleConsts() -> std::vector<prog::sym::ConstId>*;

  auto setVisibleConsts(std::vector<prog::sym::ConstId>* visibleConsts) -> void;

  [[nodiscard]] auto getParent() -> ConstBinder*;

  [[nodiscard]] auto getBoundParentConsts() const noexcept
      -> const std::vector<prog::sym::ConstId>&;

  [[nodiscard]] auto registerLocal(std::string name, prog::sym::TypeId type) -> prog::sym::ConstId;

  [[nodiscard]] auto canBind(const std::string& name) const -> bool;

  [[nodiscard]] auto doesExistButNotVisible(const std::string& name) const -> bool;

  [[nodiscard]] auto bind(const std::string& name) -> std::optional<prog::sym::ConstId>;

private:
  prog::sym::ConstDeclTable* m_consts;
  std::vector<prog::sym::ConstId>* m_visibleConsts;
  ConstBinder* m_parent;

  std::vector<prog::sym::ConstId> m_boundParentConsts;
  std::unordered_map<std::string, prog::sym::ConstId> m_boundParentConstsLookup;
};

} // namespace frontend::internal
