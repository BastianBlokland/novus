cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.1
  LANGUAGES CXX)

# Use the clang-tidy linter.
set(
  CMAKE_CXX_CLANG_TIDY
  "clang-tidy;-checks=*,-clang-analyzer-alpha.*,-fuchsia-*")

add_subdirectory(apps)
