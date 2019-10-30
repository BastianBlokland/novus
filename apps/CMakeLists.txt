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

# Lexer diagnostic tool.
add_executable(lexdiag lexdiag/main.cpp)
target_compile_features(lexdiag PRIVATE cxx_std_17)
target_compile_options(lexdiag PRIVATE -Werror -Wall -Wextra)
target_link_libraries(lexdiag PRIVATE CLI11::CLI11)
target_include_directories(lexdiag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(lexdiag PRIVATE lex)

# Parser diagnostic tool.
add_executable(parsediag parsediag/main.cpp)
target_compile_features(parsediag PRIVATE cxx_std_17)
target_compile_options(parsediag PRIVATE -Werror -Wall -Wextra)
target_link_libraries(parsediag PRIVATE CLI11::CLI11)
target_include_directories(parsediag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(parsediag PRIVATE parse)
