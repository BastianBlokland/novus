#pragma once
#include "internal/func_template.hpp"
#include <unordered_map>
#include <utility>

namespace frontend::internal {

class FuncTemplateTable final {
public:
  struct CallInfo {
    prog::sym::TypeSet argumentTypes;
    prog::sym::TypeId resultType;
    bool isAction;
  };

  FuncTemplateTable()                                 = default;
  FuncTemplateTable(const FuncTemplateTable& rhs)     = delete;
  FuncTemplateTable(FuncTemplateTable&& rhs) noexcept = default;
  ~FuncTemplateTable()                                = default;

  auto operator=(const FuncTemplateTable& rhs) -> FuncTemplateTable& = delete;
  auto operator=(FuncTemplateTable&& rhs) noexcept -> FuncTemplateTable& = default;

  auto hasFunc(const std::string& name) -> bool;

  auto declarePure(
      Context* ctx,
      const std::string& name,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode* n) -> void;

  auto declareAction(
      Context* ctx,
      const std::string& name,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode* n) -> void;

  [[nodiscard]] auto instantiate(
      const std::string& name, const prog::sym::TypeSet& typeParams, prog::OvOptions options)
      -> std::vector<const FuncTemplateInst*>;

  [[nodiscard]] auto inferParamsAndInstantiate(
      const std::string& name, const prog::sym::TypeSet& argTypes, prog::OvOptions options)
      -> std::vector<const FuncTemplateInst*>;

  [[nodiscard]] auto getCallInfo(
      const std::string& name, const prog::sym::TypeSet& typeParams, prog::OvOptions options)
      -> std::optional<CallInfo>;

  [[nodiscard]] auto inferParamsAndGetCallInfo(
      const std::string& name, const prog::sym::TypeSet& argTypes, prog::OvOptions options)
      -> std::optional<CallInfo>;

private:
  std::unordered_map<std::string, std::vector<FuncTemplate>> m_templates;

  auto declare(
      Context* ctx,
      const std::string& name,
      bool isAction,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode* n) -> void;

  auto
  inferParams(const std::string& name, const prog::sym::TypeSet& argTypes, prog::OvOptions options)
      -> std::vector<std::pair<FuncTemplate*, prog::sym::TypeSet>>;
};

} // namespace frontend::internal
