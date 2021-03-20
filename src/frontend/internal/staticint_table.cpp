#include "staticint_table.hpp"
#include "internal/context.hpp"
#include <sstream>

namespace {

auto getStaticIntName(int32_t val) -> std::string {
  std::ostringstream oss;
  oss << "#" << val;
  return oss.str();
}

} // namespace

namespace frontend::internal {

auto StaticIntTable::add(Context* ctx, prog::sym::TypeId x, prog::sym::TypeId y)
    -> prog::sym::TypeId {
  const auto xVal = getValue(x).value_or(0);
  const auto yVal = getValue(y).value_or(0);
  return getType(ctx, xVal + yVal);
}

auto StaticIntTable::sub(Context* ctx, prog::sym::TypeId x, prog::sym::TypeId y)
    -> prog::sym::TypeId {
  const auto xVal = getValue(x).value_or(0);
  const auto yVal = getValue(y).value_or(0);
  return getType(ctx, xVal - yVal);
}

auto StaticIntTable::neg(Context* ctx, prog::sym::TypeId x) -> prog::sym::TypeId {
  const auto optVal = getValue(x);
  if (!optVal) {
    return getType(ctx, 0);
  }
  return getType(ctx, -*optVal);
}

auto StaticIntTable::getValue(prog::sym::TypeId type) -> std::optional<int> {
  auto itr = m_values.find(type);
  if (itr == m_values.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto StaticIntTable::getType(Context* ctx, int32_t val) -> prog::sym::TypeId {

  auto itr = m_types.find(val);
  if (itr != m_types.end()) {
    return itr->second;
  }

  // Declare a new static-int type.
  auto name          = getStaticIntName(val);
  auto staticIntType = ctx->getProg()->declareStaticInt(name);

  // Keep track of some extra information about the type.
  ctx->declareTypeInfo(staticIntType, TypeInfo{ctx, std::move(name), input::Span{0}});

  m_types.insert({val, staticIntType});
  m_values.insert({staticIntType, val});
  return staticIntType;
}

} // namespace frontend::internal
