# NCurses cli ui library.
set(CURSES_NEED_NCURSES true)
find_package(Curses REQUIRED)

# Repl prototype.
add_executable(repl repl/main.cpp)
target_compile_features(repl PRIVATE cxx_std_17)
target_link_libraries(repl PRIVATE lex)
target_include_directories(repl PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(repl PRIVATE ${CURSES_LIBRARIES})

