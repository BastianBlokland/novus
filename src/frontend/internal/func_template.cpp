#include "func_template.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/typeinfer_expr.hpp"
#include "internal/utilities.hpp"

namespace frontend::internal {

FuncTemplate::FuncTemplate(
    const Source& src,
    prog::Program* prog,
    FuncTemplateTable* funcTemplates,
    std::string name,
    std::vector<std::string> typeParams,
    const parse::FuncDeclStmtNode& parseNode) :
    m_src{src},
    m_prog{prog},
    m_funcTemplates(funcTemplates),
    m_name{std::move(name)},
    m_typeParams{std::move(typeParams)},
    m_parseNode{parseNode} {

  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
  if (funcTemplates == nullptr) {
    throw std::invalid_argument{"Func templates table cannot be null"};
  }
}

auto FuncTemplate::getName() const -> const std::string& { return m_name; }

auto FuncTemplate::getTypeParamCount() const -> unsigned int { return m_typeParams.size(); }

auto FuncTemplate::instantiate(const prog::sym::TypeSet& typeParams) -> const FuncTemplateInst* {
  if (typeParams.getCount() != m_typeParams.size()) {
    throw std::invalid_argument{"Invalid number of type-parameters provided"};
  }

  // Check if we have a previous instantiation for the given type params.
  const auto previousInst =
      std::find_if(m_instances.begin(), m_instances.end(), [&typeParams](const auto& inst) {
        return inst->getTypeParams() == typeParams;
      });
  if (previousInst != m_instances.end()) {
    return previousInst->get();
  }

  m_instances.push_back(std::make_unique<FuncTemplateInst>(FuncTemplateInst{typeParams}));
  auto instance = m_instances.back().get();

  instantiate(instance);
  return instance;
}

auto FuncTemplate::instantiate(FuncTemplateInst* instance) -> void {
  const auto mangledName = mangleName(instance->m_typeParams);
  const auto subTable    = createSubTable(instance->m_typeParams);

  auto funcInput = getFuncInput(m_src, *m_prog, &subTable, m_parseNode, &instance->m_diags);
  if (!funcInput) {
    assert(instance->hasErrors());
    return;
  }

  auto retType = getRetType(subTable, *funcInput, &instance->m_diags);
  if (!retType) {
    assert(instance->hasErrors());
    return;
  }

  // Check if an identical function has already been registered.
  if (m_prog->lookupFunc(mangledName, *funcInput, 0)) {
    instance->m_diags.push_back(errDuplicateFuncDeclaration(m_src, m_name, m_parseNode.getSpan()));
    return;
  }

  // Declare the function in the program.
  instance->m_func = m_prog->declareUserFunc(mangledName, std::move(*funcInput), *retType);

  // Define the function.
  auto defineFuncs = DefineUserFuncs{m_src, m_prog, m_funcTemplates, &subTable};
  defineFuncs.define(*instance->m_func, m_parseNode);
  if (defineFuncs.hasErrors()) {
    const auto& defineDiags = defineFuncs.getDiags();
    assert(!defineDiags.empty());
    instance->m_diags.insert(instance->m_diags.end(), defineDiags.begin(), defineDiags.end());
    return;
  }
}

auto FuncTemplate::mangleName(const prog::sym::TypeSet& typeParams) const -> std::string {
  auto result = m_name + '_';
  for (const auto& type : typeParams) {
    const auto& typeName = m_prog->getTypeDecl(type).getName();
    result += '_' + typeName;
  }
  return result;
}

auto FuncTemplate::createSubTable(const prog::sym::TypeSet& typeParams) const
    -> TypeSubstitutionTable {
  auto subTable = TypeSubstitutionTable{};
  for (auto i = 0U; i != m_typeParams.size(); ++i) {
    subTable.declare(m_typeParams[i], typeParams[i]);
  }
  return subTable;
}

auto FuncTemplate::getRetType(
    const TypeSubstitutionTable& subTable,
    const prog::sym::TypeSet& input,
    std::vector<Diag>* diags) const -> std::optional<prog::sym::TypeId> {

  auto retType = ::frontend::internal::getRetType(m_src, *m_prog, &subTable, m_parseNode, diags);
  if (!retType) {
    return std::nullopt;
  }

  // Check if this function is a conversion.
  const auto convType = m_prog->lookupType(m_name);
  if (convType) {
    return convType;
  }

  if (retType->isInfer()) {
    // Attempt to infer the return type.
    auto constTypes = std::unordered_map<std::string, prog::sym::TypeId>{};
    for (auto i = 0U; i != input.getCount(); ++i) {
      const auto& argName = ::frontend::internal::getName(m_parseNode.getArgs()[i].getIdentifier());
      constTypes.insert({argName, input[i]});
    }

    auto inferBodyType = TypeInferExpr{m_prog, &constTypes, true};
    m_parseNode[0].accept(&inferBodyType);
    return inferBodyType.getType();
  }

  return retType;
}

} // namespace frontend::internal
