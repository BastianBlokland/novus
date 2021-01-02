#include "lex/lexer.hpp"

extern "C" auto LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) -> int {
  static_assert(sizeof(uint8_t) == sizeof(char), "Unexpected char size");

  const char* inputBegin = reinterpret_cast<const char*>(data);
  const char* inputEnd   = inputBegin + size;

  lex::lexAll(inputBegin, inputEnd);
  return 0;
}
