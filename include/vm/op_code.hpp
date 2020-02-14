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
  LoadLitInt      = 10, // [int32]  () -> (int)     Load a integer literal on the stack.
  LoadLitIntSmall = 11, // [uint8]  () -> (int)     Load a small integer literal on the stack.
  LoadLitInt0     = 12, // []       () -> (int)     Load integer 0 on the stack.
  LoadLitInt1     = 13, // []       () -> (int)     Load integer 1 on the stack.
  LoadLitFloat    = 14, // [float]  () -> (float)   Load a float literal on the stack.
  LoadLitString   = 15, // [uint32] () -> (string)  Load an item from the string literal table.

  LoadLitIp = 20, // [uint] () -> (ip) Load an instruction-pointer on the stack.

  StackAlloc = 30, // [uint8] ()    -> ()     Allocate space on the current stack-frame.
  StackStore = 31, // [uint8] (any) -> ()     Store at a offset from the stack-frame start.
  StackLoad  = 32, // [uint8] ()    -> (any)  Load from a offset from the stack-frame start.

  AddInt        = 40, // [] (int, int)          -> (int)    Add two integers.
  AddFloat      = 41, // [] (float, float)      -> (float)  Add two floats.
  AddString     = 42, // [] (string, string)    -> (string) Add two strings.
  SubInt        = 43, // [] (int, int)          -> (int)    Substract two integers.
  SubFloat      = 44, // [] (float, float)      -> (float)  Substract two floats.
  MulInt        = 45, // [] (int, int)          -> (int)    Multiply two integers.
  MulFloat      = 46, // [] (float, float)      -> (float)  Multiply two floats.
  DivInt        = 47, // [] (int, int)          -> (int)    Divide two integers.
  DivFloat      = 48, // [] (float, float)      -> (float)  Divide two floats.
  RemInt        = 49, // [] (int, int)          -> (int)    Calc division remainder of two integers.
  ModFloat      = 50, // [] (float, float)      -> (float)  Calc modulus of two floats.
  PowFloat      = 51, // [] (float, float)      -> (float)  Raise float to the power x.
  SqrtFloat     = 52, // [] (float)             -> (float)  Calc square-root of a float.
  SinFloat      = 53, // [] (float)             -> (float)  Calc sine curve at time x.
  CosFloat      = 54, // [] (float)             -> (float)  Calc cosine curve at time x.
  TanFloat      = 55, // [] (float)             -> (float)  Calc tangent curve at time x.
  ASinFloat     = 56, // [] (float)             -> (float)  Calc arc-sine curve at time x.
  ACosFloat     = 57, // [] (float)             -> (float)  Calc arc-cosine curve at time x.
  ATanFloat     = 58, // [] (float)             -> (float)  Calc arc-tangent curve at time x.
  ATan2Float    = 59, // [] (float, float)      -> (float)  Calc 2d arc-tangent at (x, y).
  NegInt        = 60, // [] (int)               -> (int)    Negate an integer.
  NegFloat      = 61, // [] (float)             -> (float)  Negate a float.
  LogicInvInt   = 62, // [] (int)               -> (int)    Logic invert, (0 to 1) & (1 to 0).
  ShiftLeftInt  = 63, // [] (int, int)          -> (int)    Bitwise shift an integer x to left.
  ShiftRightInt = 64, // [] (int, int)          -> (int)    Bitwise shift an integer x to right.
  AndInt        = 65, // [] (int, int)          -> (int)    Bitwise and two integers.
  OrInt         = 66, // [] (int, int)          -> (int)    Bitwise or two integers.
  XorInt        = 67, // [] (int, int)          -> (int)    Bitwise exclusive-or two integers.
  InvInt        = 68, // [] (int)               -> (int)    Bitwise invert an integer.
  LengthString  = 69, // [] (string)            -> (int)    Calc length of a string.
  IndexString   = 70, // [] (int, string)       -> (int)    Get char at index in string (ascii).
  SliceString   = 71, // [] (int, int, string)  -> (string) Substring from start to end (exclusive).

  CheckEqInt        = 80, // [] (int, int)                -> (int) Check integers equal.
  CheckEqFloat      = 81, // [] (float, float)            -> (int) Check floats equal.
  CheckEqString     = 82, // [] (string, string)          -> (int) Check strings equal.
  CheckEqIp         = 83, // [] (ip, ip)                  -> (int) Check instruction-pointers equal.
  CheckEqCallDynTgt = 84, // [] (ip/closure, ip/closure)  -> (int) Check ip / closures equal.
  CheckGtInt        = 85, // [] (int, int)                -> (int) Check integer is greater.
  CheckGtFloat      = 86, // [] (float, float)            -> (int) Check float is greater.
  CheckLeInt        = 87, // [] (int, int)                -> (int) Check integer is less.
  CheckLeFloat      = 88, // [] (float, float)            -> (int) Check float is less.

  ConvIntFloat    = 91, // [] (int)   -> (float)    Convert integer to float.
  ConvFloatInt    = 92, // [] (float) -> (int)      Convert float to integer.
  ConvIntString   = 93, // [] (int)   -> (string)   Convert int to string.
  ConvFloatString = 94, // [] (float) -> (string)   Convert float to string.
  ConvCharString  = 95, // [] (int)   -> (string)   Convert char to string (ascii).
  ConvIntChar     = 96, // [] (int)   -> (int)      Convert integer to char (ascii).

  MakeStruct      = 100, // [uint8] (any ...) -> (struct) Create structure containing x values.
  LoadStructField = 101, // [uint8] (struct)  -> (any)    Get value of field x in structure.

  Jump   = 220, // [ip] ()    -> () Jump to an instruction pointer.
  JumpIf = 221, // [ip] (int) -> () Jump to an instruction if integer is 1.

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

  Fail = 255, // [] () -> ()
};

auto operator<<(std::ostream& out, const OpCode& rhs) noexcept -> std::ostream&;

} // namespace vm
