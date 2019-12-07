#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class DefineUserTypes final {
public:
  DefineUserTypes() = delete;
  DefineUserTypes(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto define(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> void;
  auto define(prog::sym::TypeId id, const parse::UnionDeclStmtNode& n) -> void;

  // Check should be run after all types have been defined, is used to detect things like cyclic
  // structs.
  auto check(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> void;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;

  [[nodiscard]] auto
  getCyclicField(const prog::sym::FieldDeclTable& fields, prog::sym::TypeId rootType)
      -> std::optional<prog::sym::FieldId>;
};

} // namespace frontend::internal
