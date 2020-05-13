# Novus

[![build-test](https://img.shields.io/github/workflow/status/bastianblokland/novus/build-test/master)](https://github.com/BastianBlokland/novus/actions?query=workflow%3Abuild-test)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

**-Work in progress-**

Novus is a statically typed functional programming language, it compiles into instructions for a
stack based vm. Compiler is fully handwritten with a recursive decedent parser, multiple stages
of syntax analysis and optimizations.

Design philosophy is to keep the language simple (very little build-in types) and highly extendable
through operator overloading. Safety is also a major design goal, thats why the language is fully
type-safe, no global state, single-assignment only, fully immutable and no concept of null.

The language is designed to run on a custom stack-based vm. The vm is simple with fairly low-level
instructions. Vm is responsible for executing instructions, memory allocation, memory cleanup
(mark-sweep garbage collector) and providing a platform abstraction. Vm has very little
knowledge about types or functions so there is no runtime type introspection, instead the language
focusses on making it easier to generate code at compile time (through function and type templates).

Note this is intended as an academic exercise and not meant for production projects.

## Requirements

### Linux

* [CMake](https://cmake.org/) (At least version `3.15`).

* Modern C++ compiler (at least c++ 17) (Tested with `gcc 7.5.0` and `clang 9.0.0`).

### MacOs

* [CMake](https://cmake.org/) (At least version `3.15`).

* Modern C++ compiler (at least c++ 17) (Tested with `apple clang 11` and `clang 9.0.0`).

### Windows

* [CMake](https://cmake.org/) (At least version `3.15`).

  For example with `chocolatey`: `choco install cmake`.

Building can either be done using MinGW (windows port of Gcc and related tooling) or
Visual Studio (msvc).

#### MinGW

* [MinGW-w64](http://mingw-w64.org/). Note: Either version `7.x` or `9.x`, version `8.x` has a
[broken `std::filesystem` library impl](https://sourceforge.net/p/mingw-w64/bugs/737/).

  For example with `chocolatey`: `choco install mingw --version=7.3.0`. Unfortunately `9.x` is at
  the time of writing not yet on `chocolatey`, but the [nuwen](https://nuwen.net/mingw.html)
  distribution ships it.

### Visual Studio

* Tested with Visual Studio 2019 (make sure the c++ tooling is installed).

* For command-line building install `vswhere` (https://github.com/microsoft/vswhere).

  For example with `chocolatey`: `choco install vswhere`.

## Configure

Before building you have to configure the project, run `scripts/configure.sh` on unix or
`scripts/configure.ps1` on windows.

For Visual Studio run `scripts/configure.ps1 -Gen VS2019`, after which the Visual Studio project
can be found in the `build` directory.

On unix run `scripts/configure.sh --help` for a listing of options, on windows:
`Help scripts/configure.ps1`

## Building

After configuring the project can be build by running `scripts/build.sh` on unix or
`scripts/build.ps1` on windows.

Build output can be found in the `bin` directory.
For more convenience you can add the `bin` directory it to your `PATH`.

## Building novus source code

Novus source (`.nov`) can be compiled into novus assembly (`.nova`) using the `novc` executable.

Example: `./bin/novc examples/fizzbuzz.nov`. The output can be found at `examples/fizzbuzz.nova`.

## Running novus assembly

Novus assembly (`.nova`) can be run in the novus runtime (`novrt`).

Example: `./bin/novrt examples/fizzbuzz.nova`.

## Evaluator

Alternatively you can use the `nove` (novus evaluator) to combine the compilation and running.

You can either pass the source straight to the evaluator:
`./bin/nove 'print(pow(42, 1.337))'`.

or give a `.nov` source file to the evaluator:
`./bin/nove examples/fizzbuzz.nov`.

## Debugging

While there is no debugger (yet?) for `novus` programs, there are a few diagnostic programs:
* `novdiag-prog` (Show the generated ast, optionally after optimizing)
* `novdiag-asm` (Show the generated nov assembly code)
* `novdiag-parse` (Show the parse-tree output)
* `novdiag-lex` (Show the output tokens of the lexer)

## Testing

After building the project you can run the tests by running `scripts/test.sh` on unix or
`scripts/test.ps1` on windows.

Note: To run the compiler and vm tests they have to be enabled and build in the configure step.

Note: On windows compiler and vm tests have to be run as administrator, reason is temporary files
are created in the system root there and most users don't have access to write there.

## Ide

For basic ide support when editing `novus` source code check the `ide` directory if there is a
plugin for your ide.

For ide support when editing the compiler and vm:
* Configure the project: `make configure.debug`.
* Copy / symlink the file `build/compile_commands.json` to the root of the project.
* Use a ide extension for `cpp` support, for example: `clangd` (vscode ext: [vscode-clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd))

## Name

Naming things is hard 😅 From the Stargate tv-show: [Novus = 'new' in ancient](https://stargate.fandom.com/wiki/Novus).
