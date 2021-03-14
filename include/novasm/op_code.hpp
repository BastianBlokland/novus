#pragma once
#include <cstdint>
#include <iostream>

namespace novasm {

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

  StackAlloc      = 30, // [uint16] ()    -> ()     Allocate space on the current stack-frame.
  StackAllocSmall = 31, // [uint8 ] ()    -> ()     Allocate space on the current stack-frame.
  StackStore      = 32, // [uint16] (any) -> ()     Store at a offset from the stack-frame start.
  StackStoreSmall = 33, // [uint8 ] (any) -> ()     Store at a offset from the stack-frame start.
  StackLoad       = 34, // [uint16] ()    -> (any)  Load from a offset from the stack-frame start.
  StackLoadSmall  = 35, // [uint8 ] ()    -> (any)  Load from a offset from the stack-frame start.

  AddInt         = 40, // [] (int, int)         -> (int)    Add two ints.
  AddLong        = 41, // [] (long, long)       -> (long)   Add two longs.
  AddFloat       = 42, // [] (float, float)     -> (float)  Add two floats.
  AddString      = 43, // [] (string, string)   -> (string) Add two strings.
  AppendChar     = 44, // [] (int, string)      -> (string) Append a character to a string.
  SubInt         = 45, // [] (int, int)         -> (int)    Substract two ints.
  SubLong        = 46, // [] (long, long)       -> (long)   Substract two longs.
  SubFloat       = 47, // [] (float, float)     -> (float)  Substract two floats.
  MulInt         = 48, // [] (int, int)         -> (int)    Multiply two ints.
  MulLong        = 49, // [] (long, long)       -> (long)   Multiply two longs.
  MulFloat       = 50, // [] (float, float)     -> (float)  Multiply two floats.
  DivInt         = 51, // [] (int, int)         -> (int)    Divide two ints.
  DivLong        = 52, // [] (long, long)       -> (long)   Divide two longs.
  DivFloat       = 53, // [] (float, float)     -> (float)  Divide two floats.
  RemInt         = 54, // [] (int, int)         -> (int)    Calc division remainder of two ints.
  RemLong        = 55, // [] (long, long)       -> (long)   Calc division remainder of two longs.
  ModFloat       = 56, // [] (float, float)     -> (float)  Calc modulus of two floats.
  PowFloat       = 57, // [] (float, float)     -> (float)  Raise float to the power x.
  SqrtFloat      = 58, // [] (float)            -> (float)  Calc square-root of a float.
  SinFloat       = 59, // [] (float)            -> (float)  Calc sine curve at time x.
  CosFloat       = 60, // [] (float)            -> (float)  Calc cosine curve at time x.
  TanFloat       = 61, // [] (float)            -> (float)  Calc tangent curve at time x.
  ASinFloat      = 62, // [] (float)            -> (float)  Calc arc-sine curve at time x.
  ACosFloat      = 63, // [] (float)            -> (float)  Calc arc-cosine curve at time x.
  ATanFloat      = 64, // [] (float)            -> (float)  Calc arc-tangent curve at time x.
  ATan2Float     = 65, // [] (float, float)     -> (float)  Calc 2d arc-tangent at (x, y).
  NegInt         = 66, // [] (int)              -> (int)    Negate an int.
  NegLong        = 67, // [] (long)             -> (long)   Negate a long.
  NegFloat       = 68, // [] (float)            -> (float)  Negate a float.
  ShiftLeftInt   = 69, // [] (int, int)         -> (int)    Bitwise shift an int x to left.
  ShiftLeftLong  = 70, // [] (int, long)        -> (int)    Bitwise shift an long x to left.
  ShiftRightInt  = 71, // [] (int, int)         -> (int)    Bitwise shift an int x to right.
  ShiftRightLong = 72, // [] (int, long)        -> (int)    Bitwise shift an long x to right.
  AndInt         = 73, // [] (int, int)         -> (int)    Bitwise and two ints.
  AndLong        = 74, // [] (long, long)       -> (long)   Bitwise and two longs.
  OrInt          = 75, // [] (int, int)         -> (int)    Bitwise or two ints.
  OrLong         = 76, // [] (long, long)       -> (long)   Bitwise or two longs.
  XorInt         = 77, // [] (int, int)         -> (int)    Bitwise exclusive-or two ints.
  XorLong        = 78, // [] (long, long)       -> (long)   Bitwise exclusive-or two longs.
  InvInt         = 79, // [] (int)              -> (int)    Bitwise invert an int.
  InvLong        = 80, // [] (long)             -> (long)   Bitwise invert an long.
  LengthString   = 81, // [] (string)           -> (int)    Calc length of a string.
  IndexString    = 82, // [] (int, string)      -> (int)    Get char at index in string (ascii).
  SliceString    = 83, // [] (int, int, string) -> (string) Substring from start to end (exclusive).

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
  CheckStructNull   = 102, // [] (struct)                 -> (int) Check if struct is null.
  CheckIntZero      = 103, // [] (int)                    -> (int) Check if integer is zero.
  CheckStringEmpty  = 104, // [] (string)                 -> (int) Check if string is empty.

  ConvIntLong     = 111, // [] (int)   -> (long)     Convert int to long.
  ConvIntFloat    = 112, // [] (int)   -> (float)    Convert int to float.
  ConvLongInt     = 113, // [] (long)  -> (int)      Convert long to int.
  ConvLongFloat   = 114, // [] (long)  -> (float)    Convert long to float.
  ConvFloatInt    = 115, // [] (float) -> (int)      Convert float to int.
  ConvIntString   = 116, // [] (int)   -> (string)   Convert int to string.
  ConvLongString  = 117, // [] (long)  -> (string)   Convert long to string.
  ConvFloatString = 118, // [] (float) -> (string)   Convert float to string.
  ConvCharString  = 119, // [] (int)   -> (string)   Convert char to string (8 bit).
  ConvIntChar     = 120, // [] (int)   -> (int)      Convert int to char (8 bit).
  ConvLongChar    = 121, // [] (long)  -> (int)      Convert long to char (8 bit).
  ConvFloatChar   = 122, // [] (float) -> (int)      Convert float to char (8 bit).
  ConvFloatLong   = 123, // [] (float) -> (long)     Convert float to long.

  MakeAtomic        = 180, // [int32]        ()       -> (atomic) Create a new atomic value.
  AtomicLoad        = 181, // []             (atomic) -> (int)    Load the value of the atomic.
  AtomicCompareSwap = 182, // [int32, int32] (atomic) -> (int)    Compare and swap, return old val.
  AtomicBlock       = 183, // [int32]        (atomic) -> ()       Block until expected val.

  MakeStruct       = 190, // [uint8] (any ...)     -> (struct) Create structure containing x values.
  MakeNullStruct   = 191, // []      ()            -> struct   Create a struct without fields.
  StructLoadField  = 192, // [uint8] (struct)      -> (any)    Get value of field x in structure.
  StructStoreField = 193, // [uint8] (any, struct) -> ()       Store value at field x in structure.

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

  FutureWaitNano = 240, // [] (long, future) -> (int)  Wait x ns on future, returns if complete.
  FutureBlock    = 241, // [] (future)       -> (any)       Block and retreive result of future.
  Dup            = 243, // [] ()             -> (any)       Duplicate a value on the stack.
  Pop            = 244, // [] (any)          -> ()          Remove a value from the stack.
  Swap           = 245, // [] (any, any)     -> (any, any)  Swap the top-most two values.

  Fail = 255, // [] () -> () Terminate the current executor.
};

auto operator<<(std::ostream& out, const OpCode& rhs) noexcept -> std::ostream&;

} // namespace novasm
