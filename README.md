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

* Modern C++ compiler (Tested with `gcc 7.3.0`, `clang 9.0.0` and `apple clang 11`).
* CMake (At least version `3.15`).

## Configure

Before building you have to configure the project, either by running `scripts/configure.sh` or
executing one of the presets in the root Makefile `make configure.debug` or `make configure.release`.

By running the `scripts/configure.sh` script manually you have more control:
```
Options:
-t,--type     Build type, options: Debug, Release (default)
-d,--dir      Build directory, default: 'build'
--tests       Include compiler and runtime tests
--lint        Enable source linter
```

## Building

After configuring the project can be build by running `scripts/build.sh` or by running the preset
in the root Makefile `make build`.

By running the `scripts/build.sh` script manually you have more control:
```
Options:
-t,--threads    Number of threads to use, defaults to number of processors
-d,--dir        Build directory, default: 'build'
```

Build output can be found in the `bin` directory.

## Running

To run `novus` source code you can either pass the source straight to the evaluator:
`./bin/nove 'print(pow(42, 1.337))'`.

or give a `.nov` source file to the evaluator:
`./bin/nove examples/fizzbuzz.nov`.

For more convenience you can add the `bin` directory it to your `PATH`.

## Debugging

While there is no debugger (yet?) for `novus` programs, there are a few diagnostic programs:
* `novdiag-prog` (Show the generated ast, optionally after optimizing)
* `novdiag-asm` (Show the generated nov assembly code)
* `novdiag-parse` (Show the parse-tree output)
* `novdiag-lex` (Show the output tokens of the lexer)

## Testing

After building the project you can run the tests by running `scripts/test.sh`, or by running the
preset in the root Makefile `make test`.

Note: To run the compiler and vm tests they have to be enabled and build in the configure step.

## Ide

For basic ide support when editing `novus` source code check the `ide` directory if there is a plugin
for your ide.

For ide support when editing the compiler and vm:
* Configure the project: `make configure.debug`.
* Copy / symlink the file `build/compile_commands.json` to the root of the project.
* Use a ide extension for `cpp` support, for example: `clangd` (vscode ext: [vscode-clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd))

## Name

Naming things is hard 😅 From the Stargate tv-show: [Novus = 'new' in ancient](https://stargate.fandom.com/wiki/Novus).
