#include "internal/define_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DefineUserTypes::DefineUserTypes(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {}

auto DefineUserTypes::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto DefineUserTypes::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto DefineUserTypes::define(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> void {
  auto isValid    = true;
  auto fieldTable = prog::sym::FieldDeclTable{};
  for (const auto& field : n.getFields()) {
    // Get field type.
    const auto fieldTypeName = getName(field.getType());
    const auto fieldType     = m_prog->lookupType(fieldTypeName);
    if (!fieldType) {
      m_diags.push_back(errUndeclaredType(m_src, fieldTypeName, field.getType().getSpan()));
      isValid = false;
      continue;
    }

    // Get field identifier.
    const auto fieldName = getName(field.getIdentifier());
    if (fieldTable.lookup(fieldName)) {
      m_diags.push_back(
          errDuplicateFieldNameInStruct(m_src, fieldName, field.getIdentifier().getSpan()));
      isValid = false;
      continue;
    }
    if (m_prog->lookupType(fieldName)) {
      m_diags.push_back(
          errFieldNameConflictsWithType(m_src, fieldName, field.getIdentifier().getSpan()));
      isValid = false;
      continue;
    }

    fieldTable.registerField(fieldName, *fieldType);
  }

  if (isValid) {
    m_prog->defineUserStruct(id, std::move(fieldTable));
  }
}

} // namespace frontend::internal
