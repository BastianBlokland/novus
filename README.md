# Novus

![Build](https://github.com/BastianBlokland/novus/workflows/build-test/badge.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)

**-Work in progress-**

Novus is a statically typed functional programming language, it compiles into instructions for a
stack based vm. Compiler is fully handwritten with a recursive decedent parser, multiple stages
of syntax analysis and supports generic types and functions.

Design philosophy is to keep the language simple (very little build-in types) and highly extendable
through operator overloading. Safety is also a major design goal, thats why the language is fully
type-safe, no global state, single-assignment only, fully immutable and no concept of null.

The language is designed to run on a custom stack-based vm. The vm is simple with fairly low-level
instructions. Vm is responsible for executing instructions, memory allocation, memory cleanup
(garbage collector is in the works) and providing a platform abstraction. Vm has very little
knowledge about types or functions so there is no runtime type introspection, instead the language
focusses on making it easier to generate code at compile time (through function and type templates).

Note this is intended as an academic exercise and not meant for production projects.

## Requirements

* Modern C++ compiler (Tested with `clang 9.0.0`).
* CMake (At least version `3.15`).

## Testing

* Run CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.
* Run CTest: `(cd build & ctest)`.
* Alternatively the root MakeFile has the utility: `make test`.

## Building

* Run CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Release`.
* Build with Make: `(cd build && make)`.
* Alternatively the root MakeFile has the utilities: `make build.debug` / `make build.release`.
* Executables can be found in the `bin` directory.

## Ide

* Run CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.
* Copy / symlink the file `build/compile_commands.json` to the root of the project.
* Use `clangd` for intellisense. (vscode ext: [vscode-clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd))

## Name

Naming things is hard 😅 From the Stargate tv-show: [Novus = 'new' in ancient](https://stargate.fandom.com/wiki/Novus).
