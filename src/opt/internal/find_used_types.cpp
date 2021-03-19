#include "find_used_types.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/sym/delegate_def.hpp"
#include "prog/sym/future_def.hpp"
#include "prog/sym/lazy_def.hpp"
#include "prog/sym/struct_def.hpp"
#include "prog/sym/union_def.hpp"

namespace opt::internal {

FindUsedTypes::FindUsedTypes(const prog::Program& prog, TypeSet* types) :
    m_prog{prog}, m_types{types} {
  if (m_types == nullptr) {
    throw std::invalid_argument{"Type set cannot be null"};
  }
}

auto FindUsedTypes::markType(prog::sym::TypeId type) -> void {
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
  case prog::sym::TypeKind::Lazy: {
    const auto& lazyDef = std::get<prog::sym::LazyDef>(m_prog.getTypeDef(type));
    markType(lazyDef.getResult());
  } break;
  case prog::sym::TypeKind::Enum:
  case prog::sym::TypeKind::Int:
  case prog::sym::TypeKind::Long:
  case prog::sym::TypeKind::Float:
  case prog::sym::TypeKind::Bool:
  case prog::sym::TypeKind::String:
  case prog::sym::TypeKind::Char:
  case prog::sym::TypeKind::Stream:
  case prog::sym::TypeKind::Process:
  case prog::sym::TypeKind::StaticInt:
    break;
  }
}

auto FindUsedTypes::visitNode(const prog::expr::Node* n) -> void {
  prog::expr::DeepNodeVisitor::visitNode(n);
  markType(n->getType());
}

} // namespace opt::internal
