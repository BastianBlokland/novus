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
  ShiftLeftInt,  // Shift bits of an integer to the left.
  ShiftRightInt, // Shift bits of an integer to the right.
  AndInt,        // Bitwise and two integers.
  OrInt,         // Bitwise or two integers.
  XorInt,        // Bitwise xor two integers.
  InvInt,        // Bitwise invert an integer.
  CheckEqInt,    // Check if two integers are equal.
  CheckLeInt,    // Check if an integer is less then another integer.
  CheckLeEqInt,  // Check if an integer is less then or equal to another integer.
  CheckGtInt,    // Check if an integer is greater then another integer.
  CheckGtEqInt,  // Check if an integer is greater then or equal to another integer.
  CheckIntZero,  // Check if an integer is zero.

  AddLong,        // Add two longs.
  SubLong,        // Substract two longs.
  MulLong,        // Multiply two longs.
  DivLong,        // Divide two longs.
  RemLong,        // Return division remainder of two longs.
  NegateLong,     // Negate a long value.
  ShiftLeftLong,  // Shift bits of an long to the left.
  ShiftRightLong, // Shift bits of an long to the right.
  AndLong,        // Bitwise and two longs.
  OrLong,         // Bitwise or two longs.
  XorLong,        // Bitwise xor two longs.
  InvLong,        // Bitwise invert an long.
  CheckEqLong,    // Check if two longs are equal.
  CheckLeLong,    // Check if a long is less then another long.
  CheckLeEqLong,  // Check if a long is less then or equal to another long.
  CheckGtLong,    // Check if a long is greater then another long.
  CheckGtEqLong,  // CHeck if a long is greater then or equal to another long.

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
  CheckEqFloat,   // Check if two floats are equal.
  CheckLeFloat,   // Check if a float is less then another float.
  CheckLeEqFloat, // Check if a float is less then or equal to another float.
  CheckGtFloat,   // Check if a float is greater then another float.
  CheckGtEqFloat, // Check if a float is greater then or equal to another float.

  AddString,     // Combine two strings.
  LengthString,  // Return the length of a string.
  IndexString,   // Return the character at a specific index into a string.
  SliceString,   // Return a subsection of a string, indicated by start and end.
  CheckEqString, // Check if two strings are equal.

  AppendChar, // Append a character to a string.

  ConvIntLong,     // Convert a integer to a long.
  ConvCharLong,    // Convert a character to a long.
  ConvIntFloat,    // Convert a int to a float.
  ConvLongInt,     // Convert a long to an integer.
  ConvLongFloat,   // Convert a long to a float.
  ConvFloatInt,    // Convert a float to an integer.
  ConvIntString,   // Convert a integer to a string.
  ConvLongString,  // Convert a long to a string.
  ConvFloatString, // Convert a float to a string.
  ConvCharString,  // Convert a character to a string.
  ConvIntChar,     // Convert a integer to a character.
  ConvLongChar,    // Convert a long to a character.
  ConvFloatChar,   // Convert a float to a character.
  ConvFloatLong,   // Convert a float to long.

  MakeStruct, // Create a new structure.
  MakeUnion,  // Create a new union.

  FutureWaitNano, // Wait for x nanoseconds for a future to complete.
  FutureBlock,    // Block until a future has completed.

  LazyGet, // Retreive the value for a lazy value.

  CheckEqUserType, // Check if two user types are equal.
                   // Note: Backend will generate equality for all user-types.

  // NOTE: The source-location functions are no-ops that just return 'unknown' and '-1' at runtime.
  // If used in a supported context (like in an optional argument initializer the frontend will
  // replace these functions with literals containing the requested information).
  SourceLocFile,   // Get the filename of the current source file.
  SourceLocLine,   // Get the line number in the current source file.
  SourceLocColumn, // Get the column number in the current source file.

  ActionEndiannessNative,  // Get the native endianness of the system: Little: 0, Big: 1.
  ActionPlatformErrorCode, // Get the last plaform error.

  ActionStreamCheckValid,   // Check if a stream is valid.
  ActionStreamReadString,   // Read a string up to size x from a stream.
  ActionStreamWriteString,  // Write a string to a stream.
  ActionStreamSetOptions,   // Set options for a stream.
  ActionStreamUnsetOptions, // Unset options for a stream.

  ActionProcessStart,      // Start a new system process from the given cmdline string.
  ActionProcessBlock,      // Block until the process has exited, returns the exitcode.
  ActionProcessOpenStream, // Open a stream to stdin / stdout / stderr of the process.
  ActionProcessGetId,      // Get the native process id for the given process.
  ActionProcessSendSignal, // Send a signal to the given process.

  ActionFileOpenStream, // Open a file stream.
  ActionFileRemove,     // Delete a file from the filesystem.

  ActionTcpOpenCon,      // Open a tcp connection to a remote ip address and port.
  ActionTcpStartServer,  // Start a tcp server.
  ActionTcpAcceptCon,    // Accept a new connection from a tcp server stream.
  ActionTcpShutdown,     // Shutdown a tcp connection or server.
  ActionIpLookupAddress, // Lookup an ip address by hostname for a given address family.

  ActionConsoleOpenStream, // Open a console stream.

  ActionIsTerm,           // Check if the given stream is a terminal.
  ActionTermSetOptions,   // Set options for the terminal device.
  ActionTermUnsetOptions, // Unset option for the terminal device.
  ActionTermGetWidth,     // Get the width of the terminal device.
  ActionTermGetHeight,    // Get the height of the terminal device.

  ActionEnvGetArgCount,   // Get the amount of environment arguments passed to the application.
  ActionEnvGetArg,        // Get a environment argument string at a specific index.
  ActionEnvGetVar,        // Get a environment variable by name.
  ActionInteruptIsReq,    // Check if an interupt has been requested.
  ActionInteruptResetReq, // Reset the interupt requested flag.

  ActionClockMicroSinceEpoch, // Return a long of the amount of microseconds since epoch
                              // (01-01-1970).
  ActionClockNanoSteady,      // Return a long of the amount of nanoseconds on the steady clock.

  ActionVersionRt,       // Get the version of the runtime.
  ActionVersionCompiler, // Get the version of the compiler that created this assembly.

  ActionPlatformCode,   // Get the platform identifier: Linux: 1, MacOs: 2, Windows: 3.
  ActionWorkingDirPath, // Get the current working directory.
  ActionRtPath,         // Get the path of the runtime executable.
  ActionProgramPath,    // Get the path of the currently executing program.

  ActionSleepNano, // Sleep the executor for x nanoseconds.
  ActionFail,      // Fail the current executor (will return exit-code 1 from the application).
};

} // namespace prog::sym
