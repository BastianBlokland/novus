# Novus

**-Work in progress-**

A simple interpreted programming language meant as an exercise.

## Requirements

* Modern C++ compiler (Tested with `clang 9.0.0`).
* CMake (At least version `3.15`).

## Testing

* Run CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.
* Run CTest: `(cd build & ctest)`.

## Building

* Run CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Release`.
* Build with Make: `(cd build && make)`.
* Executables can be found in the `bin` directory.

## Ide

* Run CMake: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.
* Copy / symlink the file `build/compile_commands.json` to the root of the project.
* Use `clangd` for intellisense. (vscode ext: [vscode-clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd))

## Name

Naming things is hard 😅 From the Stargate tv-show: [Novus = 'new' in ancient](https://stargate.fandom.com/wiki/Novus).
