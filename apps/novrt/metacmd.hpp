#pragma once
#include "config.hpp"
#include <cstring>
#include <iostream>

namespace novrt {

struct MetaCmd {
  using Func = int (*)(int argc, const char** argv) noexcept;

  const char* input;
  Func func;
};

inline constexpr MetaCmd g_metaCommands[] = {
    {"--version",
     [](int, const char**) noexcept {
       std::cout << PROJECT_VER "\n";
       return 0;
     }},
    {"--help",
     [](int, const char**) noexcept {
       std::cout << "https://github.com/BastianBlokland/novus\n";
       return 0;
     }},
    {},
};

inline auto execMetaCommand(int argc, const char** argv) noexcept -> int {
  if (argc < 1) {
    return 1;
  }
  for (const MetaCmd* cmd = g_metaCommands; cmd->input; ++cmd) {
    if (strcmp(argv[0], cmd->input) == 0) {
      return cmd->func(argc - 1, argv + 1);
    }
  }
  std::cerr << "Unsupported meta command: '" << argv[0] << "'\n";
  return 1;
}

} // namespace novrt
