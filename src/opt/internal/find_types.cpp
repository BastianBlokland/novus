#include "find_types.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/sym/delegate_def.hpp"
#include "prog/sym/struct_def.hpp"
#include "prog/sym/union_def.hpp"

namespace opt::internal {

FindTypes::FindTypes(const prog::Program& prog, TypeSet* types) : m_prog{prog}, m_types{types} {
  if (m_types == nullptr) {
    throw std::invalid_argument{"Type set cannot be null"};
  }
}

auto FindTypes::markType(prog::sym::TypeId type) -> void {
  if (!m_types->insert(type).second) {
    return;
  }

  // Visit any newly found types.
  const auto& typeDecl = m_prog.getTypeDecl(type);
  switch (typeDecl.getKind()) {
  case prog::sym::TypeKind::Struct: {
    const auto& structDef = std::get<prog::sym::StructDef>(m_prog.getTypeDef(type));
    for (const auto& field : structDef.getFields()) {
      markType(field.getType());
    }
  } break;
  case prog::sym::TypeKind::Union: {
    const auto& unionDef = std::get<prog::sym::UnionDef>(m_prog.getTypeDef(type));
    for (const auto& unionType : unionDef.getTypes()) {
      markType(unionType);
    }
  } break;
  case prog::sym::TypeKind::Delegate: {
    const auto& delDef = std::get<prog::sym::DelegateDef>(m_prog.getTypeDef(type));
    for (const auto& inputType : delDef.getInput()) {
      markType(inputType);
    }
    markType(delDef.getOutput());
  } break;
  case prog::sym::TypeKind::Future: {
    const auto& futureDef = std::get<prog::sym::FutureDef>(m_prog.getTypeDef(type));
    markType(futureDef.getResult());
  } break;
  case prog::sym::TypeKind::Enum:
  case prog::sym::TypeKind::Int:
  case prog::sym::TypeKind::Long:
  case prog::sym::TypeKind::Float:
  case prog::sym::TypeKind::Bool:
  case prog::sym::TypeKind::String:
  case prog::sym::TypeKind::Char:
    break;
  }
}

auto FindTypes::visitNode(const prog::expr::Node* n) -> void {
  prog::expr::DeepNodeVisitor::visitNode(n);
  markType(n->getType());
}

} // namespace opt::internal
