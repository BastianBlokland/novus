#pragma once
#include "catch2/catch.hpp"
#include "gsl.hpp"
#include "novasm/assembler.hpp"
#include "vm/platform_interface.hpp"
#include "vm/vm.hpp"
#include <array>
#include <functional>

namespace vm {

inline auto buildExecutableExpr(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Executable {
  auto compilerVersion = std::string{"0.42.1337"};
  auto asmb            = novasm::Assembler{std::move(compilerVersion)};
  asmb.label("entrypoint");
  build(&asmb);
  asmb.addRet();
  asmb.setEntrypoint("entrypoint");
  return asmb.close();
}

inline auto buildExecutable(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Executable {
  auto compilerVersion = std::string{"0.42.1337"};
  auto asmb            = novasm::Assembler{compilerVersion};
  build(&asmb);
  return asmb.close();
}

inline auto getString(FileHandle file) -> std::string {
  constexpr size_t maxFileSize = 1024 * 1024;
  auto result                  = std::string{};
  result.resize(maxFileSize);
  if (!fileSeek(file, 0)) {
    throw std::logic_error{"Failed to seek file"};
  }
  const int bytesRead = fileRead(file, result.data(), maxFileSize);
  if (bytesRead < 0) {
    throw std::logic_error{"Failed to read file"};
  }
  result.resize(static_cast<size_t>(bytesRead));
  return result;
}

inline auto getTempFile() -> FileHandle {
  const FileHandle file = fileTemp();
  if (!fileIsValid(file)) {
    throw std::logic_error{"Failed to create temp file"};
  }
  return file;
}

inline auto prepareStdIn(std::string input) -> FileHandle {
  const FileHandle stdInFile = getTempFile();
  if (fileWrite(stdInFile, input.data(), input.size()) != static_cast<int>(input.size())) {
    throw std::logic_error{"Failed to write stdin file"};
  }
  if (!fileSeek(stdInFile, 0)) {
    throw std::logic_error{"Failed to seek stdin file"};
  }
  return stdInFile;
}

#define CHECK_ASM(ASM, INPUT, EXPECTED)                                                            \
  {                                                                                                \
    auto assembly = ASM;                                                                           \
                                                                                                   \
    const FileHandle stdInFile  = prepareStdIn(std::string{INPUT});                                \
    const FileHandle stdOutFile = getTempFile();                                                   \
                                                                                                   \
    std::array<const char*, 2> envArgs = {"Test argument 1", "Test argument 2"};                   \
    auto iface =                                                                                   \
        PlatformInterface{std::string{}, 2, envArgs.data(), stdInFile, stdOutFile, stdOutFile};    \
                                                                                                   \
    CHECK(run(&assembly, &iface) == ExecState::Success);                                           \
                                                                                                   \
    CHECK_THAT(getString(stdOutFile), Catch::Equals(EXPECTED));                                    \
                                                                                                   \
    fileClose(stdInFile);                                                                          \
    fileClose(stdOutFile);                                                                         \
  }

#define CHECK_ASM_RESULTCODE(ASM, INPUT, EXPECTED)                                                 \
  {                                                                                                \
    auto assembly = ASM;                                                                           \
                                                                                                   \
    const FileHandle stdInFile  = prepareStdIn(std::string{INPUT});                                \
    const FileHandle stdOutFile = fileInvalid();                                                   \
                                                                                                   \
    std::array<const char*, 2> envArgs = {"Test argument 1", "Test argument 2"};                   \
    auto iface =                                                                                   \
        PlatformInterface{std::string{}, 2, envArgs.data(), stdInFile, stdOutFile, stdOutFile};    \
    CHECK(run(&assembly, &iface) == (EXPECTED));                                                   \
                                                                                                   \
    fileClose(stdInFile);                                                                          \
  }

#define CHECK_EXPR(BUILD, INPUT, EXPECTED) CHECK_ASM(buildExecutableExpr(BUILD), INPUT, EXPECTED)

#define CHECK_EXPR_RESULTCODE(BUILD, INPUT, EXPECTED)                                              \
  CHECK_ASM_RESULTCODE(buildExecutableExpr(BUILD), INPUT, EXPECTED)

#define CHECK_PROG(BUILD, INPUT, EXPECTED) CHECK_ASM(buildExecutable(BUILD), INPUT, EXPECTED)

#define CHECK_PROG_RESULTCODE(BUILD, INPUT, EXPECTED)                                              \
  CHECK_ASM_RESULTCODE(buildExecutable(BUILD), INPUT, EXPECTED)

#define ADD_PRINT(ASMB)                                                                            \
  {                                                                                                \
    (ASMB)->addLoadLitInt(1); /* StdOut. */                                                        \
    (ASMB)->addPCall(novasm::PCallCode::ConsoleOpenStream);                                        \
    (ASMB)->addSwap(); /* Swap because the stream needs to be on the stack before the string. */   \
    (ASMB)->addPCall(novasm::PCallCode::StreamWriteString);                                        \
  }

} // namespace vm
