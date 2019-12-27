#pragma once
#include "internal/context.hpp"
#include "internal/struct_template.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <unordered_set>

namespace frontend::internal {

class ValidateTypes final {
public:
  ValidateTypes() = delete;
  explicit ValidateTypes(Context* context);

  auto validate(prog::sym::TypeId id) -> void;

private:
  Context* m_context;

  [[nodiscard]] auto getCyclicField(
      const prog::sym::FieldDeclTable& fields,
      const std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>& visitedTypes)
      -> std::optional<prog::sym::FieldId>;
};

} // namespace frontend::internal
