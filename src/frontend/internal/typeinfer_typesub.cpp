#include "typeinfer_typesub.hpp"

namespace frontend::internal {

auto getPathsToTypeSub(
    const std::string& subType, const parse::Type& parsetype, const TypePath& path)
    -> std::vector<TypePath> {
  const auto parseTypeName = getName(parsetype.getId());
  if (parseTypeName == subType) {
    return {path};
  }
  std::vector<TypePath> result = {};
  const auto* paramList        = parsetype.getParamList();
  if (paramList != nullptr) {
    for (auto paramInd = 0U; paramInd != paramList->getCount(); ++paramInd) {
      auto childPath = path;
      childPath.emplace_back(parseTypeName, paramInd);
      auto childResult = getPathsToTypeSub(subType, (*paramList)[paramInd], childPath);
      result.insert(result.end(), childResult.begin(), childResult.end());
    }
  }
  return result;
}

auto resolvePathToTypeSub(
    const Context& ctx,
    TypePath::const_iterator begin,
    const TypePath::const_iterator& end,
    prog::sym::TypeId type) -> std::optional<prog::sym::TypeId> {
  if (begin == end) {
    return type;
  }
  const auto info = ctx.getTypeInfo(type);
  if (!info || !info->hasParams()) {
    return std::nullopt;
  }
  const auto& params     = *info->getParams();
  const auto& targetName = begin->first;
  if (targetName != info->getName()) {
    return std::nullopt;
  }
  const auto& targetIndex = begin->second;
  if (targetIndex >= params.getCount()) {
    return std::nullopt;
  }
  return resolvePathToTypeSub(ctx, ++begin, end, params[targetIndex]);
}

} // namespace frontend::internal
