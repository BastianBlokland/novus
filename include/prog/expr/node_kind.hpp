#pragma once

namespace prog::expr {

enum class NodeKind {
  Assign,
  CallDyn,
  CallSelf,
  Call,
  Closure,
  Const,
  Fail,
  Field,
  Group,
  LitBool,
  LitChar,
  LitEnum,
  LitFloat,
  LitFunc,
  LitInt,
  LitLong,
  LitString,
  Switch,
  UnionCheck,
  UnionGet,
};

} // namespace prog::expr
