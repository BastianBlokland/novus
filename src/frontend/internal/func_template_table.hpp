#pragma once
#include "frontend/source.hpp"
#include "internal/func_template.hpp"
#include "prog/program.hpp"
#include <unordered_map>

namespace frontend::internal {

class FuncTemplateTable final {
public:
  FuncTemplateTable(const Source& src, prog::Program* prog);

  FuncTemplateTable()                                 = delete;
  FuncTemplateTable(const FuncTemplateTable& rhs)     = delete;
  FuncTemplateTable(FuncTemplateTable&& rhs) noexcept = default;
  ~FuncTemplateTable()                                = default;

  auto operator=(const FuncTemplateTable& rhs) -> FuncTemplateTable& = delete;
  auto operator=(FuncTemplateTable&& rhs) noexcept -> FuncTemplateTable& = delete;

  auto declare(
      const std::string& name, std::vector<std::string> typeSubs, const parse::FuncDeclStmtNode& n)
      -> void;

  auto instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
      -> std::vector<const FuncTemplateInst*>;

  auto getRetType(const std::string& name, const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeId>;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::unordered_map<std::string, std::vector<FuncTemplate>> m_templates;
};

} // namespace frontend::internal
