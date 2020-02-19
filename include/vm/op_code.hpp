#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

/*
  Items in brackets '[]' are the arguments for instructions.
  Items in parentheses '()' on the left side of '->' are taken from the stack and on the right side
  are pushed onto the stack.
*/

enum class OpCode : uint8_t {
  LoadLitInt      = 10, // [int32]  () -> (int)     Load a int literal on the stack.
  LoadLitIntSmall = 11, // [uint8]  () -> (int)     Load a small int literal on the stack.
  LoadLitInt0     = 12, // []       () -> (int)     Load int 0 on the stack.
  LoadLitInt1     = 13, // []       () -> (int)     Load int 1 on the stack.
  LoadLitLong     = 14, // [int64]  () -> (long)    Load a long literal on the stack.
  LoadLitFloat    = 15, // [float]  () -> (float)   Load a float literal on the stack.
  LoadLitString   = 16, // [uint32] () -> (string)  Load an item from the string literal table.

  LoadLitIp = 20, // [uint] () -> (ip) Load an instruction-pointer on the stack.

  StackAlloc = 30, // [uint8] ()    -> ()     Allocate space on the current stack-frame.
  StackStore = 31, // [uint8] (any) -> ()     Store at a offset from the stack-frame start.
  StackLoad  = 32, // [uint8] ()    -> (any)  Load from a offset from the stack-frame start.

  AddInt        = 40, // [] (int, int)          -> (int)    Add two ints.
  AddLong       = 41, // [] (long, long)        -> (long)   Add two longs.
  AddFloat      = 42, // [] (float, float)      -> (float)  Add two floats.
  AddString     = 43, // [] (string, string)    -> (string) Add two strings.
  SubInt        = 44, // [] (int, int)          -> (int)    Substract two ints.
  SubLong       = 45, // [] (long, long)        -> (long)   Substract two longs.
  SubFloat      = 46, // [] (float, float)      -> (float)  Substract two floats.
  MulInt        = 47, // [] (int, int)          -> (int)    Multiply two ints.
  MulLong       = 48, // [] (long, long)        -> (long)   Multiply two longs.
  MulFloat      = 49, // [] (float, float)      -> (float)  Multiply two floats.
  DivInt        = 50, // [] (int, int)          -> (int)    Divide two ints.
  DivLong       = 51, // [] (long, long)        -> (long)   Divide two longs.
  DivFloat      = 52, // [] (float, float)      -> (float)  Divide two floats.
  RemInt        = 53, // [] (int, int)          -> (int)    Calc division remainder of two ints.
  RemLong       = 54, // [] (long, long)        -> (long)   Calc division remainder of two longs.
  ModFloat      = 55, // [] (float, float)      -> (float)  Calc modulus of two floats.
  PowFloat      = 56, // [] (float, float)      -> (float)  Raise float to the power x.
  SqrtFloat     = 57, // [] (float)             -> (float)  Calc square-root of a float.
  SinFloat      = 58, // [] (float)             -> (float)  Calc sine curve at time x.
  CosFloat      = 59, // [] (float)             -> (float)  Calc cosine curve at time x.
  TanFloat      = 60, // [] (float)             -> (float)  Calc tangent curve at time x.
  ASinFloat     = 61, // [] (float)             -> (float)  Calc arc-sine curve at time x.
  ACosFloat     = 62, // [] (float)             -> (float)  Calc arc-cosine curve at time x.
  ATanFloat     = 63, // [] (float)             -> (float)  Calc arc-tangent curve at time x.
  ATan2Float    = 64, // [] (float, float)      -> (float)  Calc 2d arc-tangent at (x, y).
  NegInt        = 65, // [] (int)               -> (int)    Negate an int.
  NegLong       = 66, // [] (long)              -> (long)   Negate a long.
  NegFloat      = 67, // [] (float)             -> (float)  Negate a float.
  LogicInvInt   = 68, // [] (int)               -> (int)    Logic invert, (0 to 1) & (1 to 0).
  ShiftLeftInt  = 69, // [] (int, int)          -> (int)    Bitwise shift an int x to left.
  ShiftRightInt = 70, // [] (int, int)          -> (int)    Bitwise shift an int x to right.
  AndInt        = 71, // [] (int, int)          -> (int)    Bitwise and two ints.
  OrInt         = 72, // [] (int, int)          -> (int)    Bitwise or two ints.
  XorInt        = 73, // [] (int, int)          -> (int)    Bitwise exclusive-or two ints.
  InvInt        = 74, // [] (int)               -> (int)    Bitwise invert an int.
  LengthString  = 75, // [] (string)            -> (int)    Calc length of a string.
  IndexString   = 76, // [] (int, string)       -> (int)    Get char at index in string (ascii).
  SliceString   = 77, // [] (int, int, string)  -> (string) Substring from start to end (exclusive).

  CheckEqInt        = 90, //  [] (int, int)               -> (int) Check ints equal.
  CheckEqLong       = 91, //  [] (long, long)             -> (int) Check long equal.
  CheckEqFloat      = 92, //  [] (float, float)           -> (int) Check floats equal.
  CheckEqString     = 93, //  [] (string, string)         -> (int) Check strings equal.
  CheckEqIp         = 94, //  [] (ip, ip)                 -> (int) Check instruction-pointers equal.
  CheckEqCallDynTgt = 95, //  [] (ip/closure, ip/closure) -> (int) Check ip / closures equal.
  CheckGtInt        = 96, //  [] (int, int)               -> (int) Check int is greater.
  CheckGtLong       = 97, //  [] (long, long)             -> (int) Check int is greater.
  CheckGtFloat      = 98, //  [] (float, float)           -> (int) Check float is greater.
  CheckLeInt        = 99, //  [] (int, int)               -> (int) Check int is less.
  CheckLeLong       = 100, // [] (long, long)             -> (int) Check int is less.
  CheckLeFloat      = 101, // [] (float, float)           -> (int) Check float is less.

  ConvIntLong     = 111, // [] (int)   -> (long)     Convert int to long.
  ConvIntFloat    = 112, // [] (int)   -> (float)    Convert int to float.
  ConvLongInt     = 113, // [] (long)  -> (int)      Convert long to int.
  ConvFloatInt    = 114, // [] (float) -> (int)      Convert float to int.
  ConvIntString   = 115, // [] (int)   -> (string)   Convert int to string.
  ConvLongString  = 116, // [] (long)  -> (string)   Convert long to string.
  ConvFloatString = 117, // [] (float) -> (string)   Convert float to string.
  ConvCharString  = 118, // [] (int)   -> (string)   Convert char to string (ascii).
  ConvIntChar     = 119, // [] (int)   -> (int)      Convert int to char (ascii).

  MakeStruct      = 190, // [uint8] (any ...) -> (struct) Create structure containing x values.
  LoadStructField = 191, // [uint8] (struct)  -> (any)    Get value of field x in structure.

  Jump   = 220, // [ip] ()    -> () Jump to an instruction pointer.
  JumpIf = 221, // [ip] (int) -> () Jump to an instruction if int is not 0.

  Call          = 230, // [uint8, ip] (any ...)             -> (any)    Call with x args.
  CallTail      = 231, // [uint8, ip] (any ...)             -> (any)    Tail-call with x args.
  CallForked    = 232, // [uint8, ip] (any ...)             -> (future) Fork-call with x args.
  CallDyn       = 234, // [uint8]     (ip/closure, any ...) -> (any)    Dynamic-call with x args.
  CallDynTail   = 235, // [uint8]     (ip/closure, any ...) -> (any)    Dynamic-tail-call with x.
  CallDynForked = 236, // [uint8]     (ip/closure, any ...) -> (future) Dynamic-fork-call with x.
  PCall         = 237, // [pcallcode] (any ...)             -> (any)    Execute platform call.
  Ret           = 238, // []          (any)                 -> ()       Return from stack-frame.

  FutureWait  = 240, // [] (int, future)  -> (int)  Wait x ms on future, returns if complete.
  FutureBlock = 241, // [] (future)       -> (any)  Block and retreive result of future.
  Dup         = 243, // [] ()             -> (any)  Duplicate a value on the stack.
  Pop         = 244, // [] (any)          -> ()     Remove a value from the stack.

  Fail = 255, // [] () -> () Terminate the current executor.
};

auto operator<<(std::ostream& out, const OpCode& rhs) noexcept -> std::ostream&;

} // namespace vm
