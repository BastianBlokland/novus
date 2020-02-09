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

# Evaluator.
add_executable(eval eval/main.cpp)
target_compile_features(eval PRIVATE cxx_std_17)
target_compile_options(eval PRIVATE -fexceptions)
target_link_libraries(eval PRIVATE CLI11::CLI11)
target_include_directories(eval PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(eval PRIVATE frontend)
target_link_libraries(eval PRIVATE backend)
target_link_libraries(eval PRIVATE vm)

# Lexer diagnostic tool.
add_executable(lexdiag lexdiag/main.cpp)
target_compile_features(lexdiag PRIVATE cxx_std_17)
target_compile_options(lexdiag PRIVATE -fexceptions)
target_link_libraries(lexdiag PRIVATE CLI11::CLI11)
target_include_directories(lexdiag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(lexdiag PRIVATE lex)

# Parser diagnostic tool.
add_executable(parsediag parsediag/main.cpp)
target_compile_features(parsediag PRIVATE cxx_std_17)
target_compile_options(parsediag PRIVATE -fexceptions)
target_link_libraries(parsediag PRIVATE CLI11::CLI11)
target_include_directories(parsediag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(parsediag PRIVATE parse)

# Program diagnostic tool.
add_executable(progdiag progdiag/main.cpp)
target_compile_features(progdiag PRIVATE cxx_std_17)
target_compile_options(progdiag PRIVATE -fexceptions)
target_link_libraries(progdiag PRIVATE CLI11::CLI11)
target_include_directories(progdiag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(progdiag PRIVATE frontend)

# Assembly diagnostic tool.
add_executable(asmdiag asmdiag/main.cpp)
target_compile_features(asmdiag PRIVATE cxx_std_17)
target_compile_options(asmdiag PRIVATE -fexceptions)
target_link_libraries(asmdiag PRIVATE CLI11::CLI11)
target_include_directories(asmdiag PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(asmdiag PRIVATE frontend)
target_link_libraries(asmdiag PRIVATE backend)
target_link_libraries(asmdiag PRIVATE vm)
