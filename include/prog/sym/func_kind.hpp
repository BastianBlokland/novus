#pragma once

namespace prog::sym {

// Function kind.
// Note: All non 'User' kinds are build-in functions where the backend explicitly knows what
// assembly to generate.
enum class FuncKind {
  NoOp, // Backend will generate no assembly, usefull for aliasing conversions.
  User, // User-defined function, contains a definition in the 'FuncDefTable'.

  AddInt,        // Add two integers.
  SubInt,        // Substact two integers.
  MulInt,        // Multiply two integers.
  DivInt,        // Divide two integers.
  RemInt,        // Return division remainder of two integers.
  NegateInt,     // Negate an integer.
  IncrementInt,  // Increment an integer.
  DecrementInt,  // Decrement an integer.
  ShiftLeftInt,  // Shift bits of an integer to the left.
  ShiftRightInt, // Shift bits of an integer to the right.
  AndInt,        // Bitwise and two integers.
  OrInt,         // Bitwise or two integers.
  XorInt,        // Bitwise xor two integers.
  InvInt,        // Bitwise invert an integer.
  CheckEqInt,    // Check if two integers are equal.
  CheckNEqInt,   // Check if two integers are not equal.
  CheckLeInt,    // Check if an integer is less then another integer.
  CheckLeEqInt,  // Check if an integer is less then or equal to another integer.
  CheckGtInt,    // Check if an integer is greater then another integer.
  CheckGtEqInt,  // Check if an integer is greater then or equal to another integer.

  AddLong,       // Add two longs.
  SubLong,       // Substract two longs.
  MulLong,       // Multiply two longs.
  DivLong,       // Divide two longs.
  RemLong,       // Return division remainder of two longs.
  NegateLong,    // Negate a long value.
  IncrementLong, // Increment a long value.
  DecrementLong, // Decrement a long value.
  CheckEqLong,   // Check if two longs are equal.
  CheckNEqLong,  // Check if two longs are not equal.
  CheckLeLong,   // Check if a long is less then another long.
  CheckLeEqLong, // Check if a long is less then or equal to another long.
  CheckGtLong,   // Check if a long is greater then another long.
  CheckGtEqLong, // CHeck if a long is greater then or equal to another long.

  AddFloat,       // Add two floats.
  SubFloat,       // Substract two floats.
  MulFloat,       // Multiply two floats.
  DivFloat,       // Divide two floats.
  ModFloat,       // Modulo two floats.
  PowFloat,       // Raise a float to the power of another float.
  SqrtFloat,      // Return the square-root of a float.
  SinFloat,       // Compute the sine of a float.
  CosFloat,       // Compute the cosine of a float.
  TanFloat,       // Compute the tangent of a float.
  ASinFloat,      // Compute the arc sine of a float.
  ACosFloat,      // Compute the arc cosine of a float.
  ATanFloat,      // Compute the arc tangent of a float.
  ATan2Float,     // Compute the arc tangent of two floats.
  NegateFloat,    // Negate a float.
  IncrementFloat, // Increment a float.
  DecrementFloat, // Decrement a float.
  CheckEqFloat,   // Check if two floats are equal.
  CheckNEqFloat,  // Check if two floats are not equal.
  CheckLeFloat,   // Check if a float is less then another float.
  CheckLeEqFloat, // Check if a float is less then or equal to another float.
  CheckGtFloat,   // Check if a float is greater then another float.
  CheckGtEqFloat, // Check if a float is greater then or equal to another float.

  InvBool,      // Logic invert a boolean, true -> false, false -> true.
  CheckEqBool,  // Check if two booleans are equal.
  CheckNEqBool, // Check if two booleans are not equal.

  AddString,      // Combine two strings.
  LengthString,   // Return the length of a string.
  IndexString,    // Return the character at a specific index into a string.
  SliceString,    // Return a subsection of a string, indicated by start and end.
  CheckEqString,  // Check if two strings are equal.
  CheckNEqString, // Check if two strings are not equal.

  CombineChar,   // Combine two characters into a string.
  AppendChar,    // Append a character to a string.
  IncrementChar, // Increment a character.
  DecrementChar, // Decrement a character.

  ConvIntLong,     // Convert a integer to a long.
  ConvCharLong,    // Convert a character to a long.
  ConvIntFloat,    // Convert a int to a float.
  ConvLongInt,     // Convert a long to an integer.
  ConvLongFloat,   // Convert a long to a float.
  ConvFloatInt,    // Convert a float to an integer.
  ConvIntString,   // Convert a integer to a string.
  ConvLongString,  // Convert a long to a string.
  ConvFloatString, // Convert a float to a string.
  ConvBoolString,  // Convert a boolean to a string.
  ConvCharString,  // Convert a character to a string.
  ConvIntChar,     // Convert a integer to a character.
  ConvFloatChar,   // Convert a float to a character.
  ConvFloatLong,   // Convert a float to long.

  DefInt,    // Return the default value for a integer.
  DefLong,   // Return the default value for a long.
  DefFloat,  // Return the default value for a float.
  DefBool,   // Return the default value for a boolean.
  DefString, // Return the default value for a string.

  MakeStruct, // Create a new structure.
  MakeUnion,  // Create a new union.

  FutureWaitNano, // Wait for x nanoseconds for a future to complete.
  FutureBlock,    // Block until a future has completed.

  LazyGet, // Retreive the value for a lazy value.

  CheckEqUserType,  // Check if two user types are equal. Note: Backend will generate equality for
                    // all user-types.
  CheckNEqUserType, // Check if two user-types are not equal.

  ActionStreamCheckValid,   // Check if a stream is valid.
  ActionStreamReadString,   // Read a string up to size x from a stream.
  ActionStreamReadChar,     // Read a single character from a stream.
  ActionStreamWriteString,  // Write a string to a stream.
  ActionStreamWriteChar,    // Write a single character to a stream.
  ActionStreamFlush,        // Flush a stream to the underlying device.
  ActionStreamSetOptions,   // Set options for a stream.
  ActionStreamUnsetOptions, // Unset options for a stream.

  ActionProcessStart, // Start a new system process from the given cmdline string.
  ActionProcessBlock, // Block until the process has exited, returns the exitcode.

  ActionFileOpenStream, // Open a file stream.
  ActionFileRemove,     // Delete a file from the filesystem.

  ActionTcpOpenCon,      // Open a tcp connection to a remote ip address and port.
  ActionTcpStartServer,  // Start a tcp server.
  ActionTcpAcceptCon,    // Accept a new connection from a tcp server stream.
  ActionIpLookupAddress, // Lookup an ip address by hostname.

  ActionConsoleOpenStream, // Open a console stream.

  ActionTermSetOptions,   // Set options for the terminal device.
  ActionTermUnsetOptions, // Unset option for the terminal device.
  ActionTermGetWidth,     // Get the width of the terminal device.
  ActionTermGetHeight,    // Get the height of the terminal device.

  ActionGetEnvArgCount,   // Get the amount of environment arguments passed to the application.
  ActionGetEnvArg,        // Get a environment argument string at a specific index.
  ActionGetEnvVar,        // Get a environment variable by name.
  ActionInteruptIsReq,    // Check if an interupt has been requested.
  ActionInteruptResetReq, // Reset the interupt requested flag.

  ActionClockMicroSinceEpoch, // Return a long of the amount of microseconds since epoch
                              // (01-01-1970).
  ActionClockNanoSteady,      // Return a long of the amount of nanoseconds on the steady clock.

  ActionSleepNano, // Sleep the executor for x nanoseconds.
  ActionAssert,    // Assert a condition to be true (and fail the executor if it isn't).
  ActionFail,      // Fail the current executor (will return exit-code 1 from the application).
};

} // namespace prog::sym
