#include "static_int_table.hpp"
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
  return staticIntType;
}

} // namespace frontend::internal
