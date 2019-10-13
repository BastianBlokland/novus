include(FetchContent)

# Cli11 command line parser.
FetchContent_Declare(
  cli11
  GIT_REPOSITORY git@github.com:CLIUtils/CLI11.git
  GIT_TAG v1.8.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cli11)

# Rang library for colored command line output.
FetchContent_Declare(
  rang
  GIT_REPOSITORY git@github.com:agauniyal/rang.git
  GIT_TAG v3.1.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(rang)

# NCurses cli ui library.
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

# Lexer diagnostic tool.
add_executable(lexdiag lexdiag/main.cpp)
target_compile_features(lexdiag PRIVATE cxx_std_17)
target_compile_options(lexdiag PRIVATE -Werror -Wall -Wextra)
target_link_libraries(lexdiag PRIVATE CLI11::CLI11)
target_include_directories(lexdiag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(lexdiag PRIVATE lex)

# Repl prototype.
add_executable(repl repl/main.cpp)
target_compile_features(repl PRIVATE cxx_std_17)
target_compile_options(repl PRIVATE -Werror -Wall -Wextra)
target_link_libraries(repl PRIVATE lex)
target_include_directories(repl PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(repl PRIVATE ${CURSES_LIBRARIES})
