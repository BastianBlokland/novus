#pragma once
#include "catch2/catch.hpp"
#include "gsl.hpp"
#include "novasm/assembler.hpp"
#include "vm/platform_interface.hpp"
#include "vm/vm.hpp"
#include <array>
#include <cstdio>
#include <functional>

namespace vm {

inline auto buildAssemblyExpr(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto compilerVersion = std::string{"0.42.1337"};
  auto asmb            = novasm::Assembler{std::move(compilerVersion)};
  asmb.label("entrypoint");
  build(&asmb);
  asmb.addRet();
  asmb.setEntrypoint("entrypoint");
  return asmb.close();
}

inline auto buildAssembly(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto compilerVersion = std::string{"0.42.1337"};
  auto asmb            = novasm::Assembler{compilerVersion};
  build(&asmb);
  return asmb.close();
}

inline auto getString(FILE* file) -> std::string {
  std::fseek(file, 0, SEEK_END);
  auto size = std::ftell(file);

  auto result = std::string{};
  result.resize(size);

  std::rewind(file);
  const auto readAmt = std::fread(&result[0], 1, size, file);
  REQUIRE(static_cast<long>(readAmt) == size);
  return result;
}

inline auto makeTmpFile() -> FILE* {
  gsl::owner<std::FILE*> stdInFile = std::tmpfile();
  if (stdInFile == nullptr) {
    throw std::logic_error{"Failed to create temporary file, run tests with admin rights"};
  }
  return stdInFile;
}

#define CHECK_ASM(ASM, INPUT, EXPECTED)                                                            \
  {                                                                                                \
    auto assembly = ASM;                                                                           \
                                                                                                   \
    /* Open temporary files to use as stdIn and StdOut. */                                         \
    gsl::owner<std::FILE*> stdInFile = makeTmpFile();                                              \
    std::string input                = INPUT;                                                      \
    std::fwrite(input.data(), input.size(), 1, stdInFile);                                         \
    std::fflush(stdInFile);                                                                        \
    std::fseek(stdInFile, 0, SEEK_SET);                                                            \
                                                                                                   \
    gsl::owner<std::FILE*> stdOutFile = makeTmpFile();                                             \
                                                                                                   \
    std::array<const char*, 2> envArgs = {"Test argument 1", "Test argument 2"};                   \
    auto iface =                                                                                   \
        PlatformInterface{std::string{}, 2, envArgs.data(), stdInFile, stdOutFile, nullptr};       \
                                                                                                   \
    CHECK(run(&assembly, &iface) == ExecState::Success);                                           \
                                                                                                   \
    /* Verify that the correct stdOut output was generated. */                                     \
    CHECK_THAT(getString(stdOutFile), Catch::Equals(EXPECTED));                                    \
                                                                                                   \
    /* Close the temporary files (they will auto destruct). */                                     \
    std::fclose(stdInFile);                                                                        \
    std::fclose(stdOutFile);                                                                       \
  }

#define CHECK_ASM_RESULTCODE(ASM, INPUT, EXPECTED)                                                 \
  {                                                                                                \
    auto assembly = ASM;                                                                           \
                                                                                                   \
    /* Open temporary file to use as stdIn. */                                                     \
    gsl::owner<std::FILE*> stdInFile = makeTmpFile();                                              \
    std::string input                = INPUT;                                                      \
    std::fwrite(input.data(), input.size(), 1, stdInFile);                                         \
    std::fflush(stdInFile);                                                                        \
    std::fseek(stdInFile, 0, SEEK_SET);                                                            \
                                                                                                   \
    std::array<const char*, 2> envArgs = {"Test argument 1", "Test argument 2"};                   \
    auto iface = PlatformInterface{std::string{}, 2, envArgs.data(), stdInFile, nullptr, nullptr}; \
    CHECK(run(&assembly, &iface) == (EXPECTED));                                                   \
                                                                                                   \
    /* Close the temporary file (it will auto destruct). */                                        \
    std::fclose(stdInFile);                                                                        \
  }

#define CHECK_EXPR(BUILD, INPUT, EXPECTED) CHECK_ASM(buildAssemblyExpr(BUILD), INPUT, EXPECTED)

#define CHECK_EXPR_RESULTCODE(BUILD, INPUT, EXPECTED)                                              \
  CHECK_ASM_RESULTCODE(buildAssemblyExpr(BUILD), INPUT, EXPECTED)

#define CHECK_PROG(BUILD, INPUT, EXPECTED) CHECK_ASM(buildAssembly(BUILD), INPUT, EXPECTED)

#define CHECK_PROG_RESULTCODE(BUILD, INPUT, EXPECTED)                                              \
  CHECK_ASM_RESULTCODE(buildAssembly(BUILD), INPUT, EXPECTED)

#define ADD_PRINT(ASMB)                                                                            \
  {                                                                                                \
    (ASMB)->addLoadLitInt(1); /* StdOut. */                                                        \
    (ASMB)->addPCall(novasm::PCallCode::ConsoleOpenStream);                                        \
    (ASMB)->addSwap(); /* Swap because the stream needs to be on the stack before the string. */   \
    (ASMB)->addPCall(novasm::PCallCode::StreamWriteString);                                        \
  }

#define ADD_PRINT_CHAR(ASMB)                                                                       \
  {                                                                                                \
    (ASMB)->addLoadLitInt(1); /* StdOut. */                                                        \
    (ASMB)->addPCall(novasm::PCallCode::ConsoleOpenStream);                                        \
    (ASMB)->addSwap(); /* Swap because the stream needs to be on the stack before the char. */     \
    (ASMB)->addPCall(novasm::PCallCode::StreamWriteChar);                                          \
  }

#if defined(_WIN32)
#define STR_NEWLINE "\r\n"
#else // !_WIN32
#define STR_NEWLINE "\n"
#endif

} // namespace vm
