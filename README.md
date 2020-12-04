# Novus

[![build-test](https://img.shields.io/github/workflow/status/bastianblokland/novus/build-test/master)](https://github.com/BastianBlokland/novus/actions?query=workflow%3Abuild-test)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

Novus is a general purpose, statically typed, functional programming language.
Novus source compiles into instructions for the Novus virtual machine which provides the runtime
(eg Garbage collection) and platform abstraction (eg io).
The runtime supports Linux, Mac and Windows at this time.

Note: This is intended as an academic exercise and not meant for production projects.

# Intro

Novus is an expression based pure functional language with eager evaluation. In practice this means
that all types are immutable, normal functions cannot have side-effects and all functions have to
produce a value.

Simplest function:
```n
fun square(int x) x * x
```
Input parameters have to be typed manually but the return type is inferred from the expression.

Advantage of providing type information for the input parameters is that functions can be overloaded:
```n
fun getTxt(string s)  s
fun getTxt(bool b)    b ? "true" : "false"
fun getTxt(User u)    n.name
```

Expressions can be combined using the group operator ';' (similar to the comma operator in some
languages). In an expression group the result is the value of the last expression.

Can be used to define a constant and reuse it in the next expression:
```n
fun cube(int x)
  sqr = x * x; sqr * x
```
Note: All 'variables' are single assignment only, so in the above example `sqr` cannot be redefined
to mean something else later.

## Control flow

### Conditional operator
For simple control-flow the conditional-operator (aka ternary-operator) can be used:
```n
fun max(int a, int b)
  a > b ? a : b
```

### Switch expression
For more elaborate control-flow there is the switch expression:
```n
fun fizzbuzz(int i)
  fizz = i % 3 == 0;
  buzz = i % 5 == 0;
  if fizz && buzz -> "FizzBuzz"
  if fizz         -> "Fizz"
  if buzz         -> "Buzz"
  else            -> string(i)
```
The switch expression is very similar to 'if' statements in imperative languages, however in
Novus its an expression so the entire switch has to produce a value of a single type (meaning
it has to be exhaustive, so an 'else' is required if the compiler cannot guarantee the if conditions
being exhaustive).

Note: there are no loops in Novus, instead iterating is done using recursion. When performing tail
recursion the runtime guarantees to execute it in constant stack space.

## Type system

The Novus type system contains some basic types build in to the language:
* `int` (32 bit signed integer)
* `long` (64 bit signed integer)
* `float` (32 bit floating point number)
* `bool`
* `string`
* `char` (8 bit unsigned integer)

(Plus a few more niche types: `sys_stream`, `sys_process`, `function`, `action`, `future` and `lazy`).

Note: These are the types the language itself supports, there are however many more types
implemented in the [standard library](https://github.com/BastianBlokland/novus/tree/master/novstd).

And can be extended with three kinds of user defined types:

### Struct (aka record)
```n
struct User =
  string  name,
  int     age

fun getDefaultUser()
  User("John doe", 32)

fun getName(User u)
  u.name
```

### Union (aka discriminated union or tagged union)
```n
union IntOrBool = int, bool

fun getVal(int i)
  i == 0 ? false : i

fun getNum(IntOrBool ib)
  if ib as int  i -> i
  if ib as bool b -> b ? 1 : 0
```
Note that there is no 'else' in the last switch expression, this is allowed because the compiler
can guarantee that the if conditions are exhaustive.

### Enum (aka enumeration)
```n
enum WeekDay =
  Monday    : 1,
  Tuesday   : 2,
  Wednesday : 3,
  Thursday,
  Friday,
  Saturday,
  Sunday

fun sunday()
  WeekDay.Sunday

fun isSunday(WeekDay wd)
  wd == WeekDay.Sunday
```
Enum's follow the conventions that most c-like languages have, they are named values. If no value
is provided the last value is automatically incremented by one (starting from 0).

## Generic programming (type and function templates)

To aid in generic programming you can create type and function templates
(similar in spirit to c++ templates). Instead of angle brackets '<>' found in many other
languages to define a type set, Novus uses curly braces '{}'.

### Type template
```n
struct Pair{T1, T2} =
  T1 first,
  T2 second
```

Instantiation of a type template:
```n
fun sum(float a, float b)
  sum(Pair(a, b))

fun sum(Pair{float, float} p)
  p.first + p.second
```
Note: When constructing a type the type parameters can be inferred from usage.

### Function template
```n
fun sum{T}(T a, T b)
  a + b

fun onePlusTwo()
  sum(1, 2)
```
Note: When calling a templated function most of the time the type parameters can be inferred.

## Operator overloading

Operators can defined like any other function.
```n
struct Pair = int first, int second

fun +(Pair a, Pair b)
  Pair(a.first + b.first, a.second + b.second)

fun [](Pair p, int i)
  i == 0 ? p.first : p.second

fun sum(Pair a, Pair b)
  a + b

fun getFirst(Pair p)
  p[0]
```

The following list of operators can be overloaded:
`+`, `++`, `-`, `--`, `*`, `/`, `%`, `&`, `|`, `!`, `!!`, `<<`, `>>`, `^`, `~`, `==`, `!=`, `<`, `>`,
`<=`, `>=`, `::`, `[]`, `()`, `?`, `??`.

Note: All operators are left associative except for the `::` operator, which makes the `::` operator
ideal for creating linked lists.

## First class functions

Functions can be passed as values using the build-in `function{}` type template.
The last type in the type-set is the return-type, the types before that are the input types.
```n
fun performOp(int val, function{int, int} op)
  op(val)

fun square(int v) v * v
fun cube(int v)   v * v * v

print(performOp(43, cube))
print(performOp(43, square))
```

### Lambda's (aka anonymous functions)

Anonymous functions can be defined using the lambda syntax.
```n
fun performOp(int val, function{int, int} op)
  op(val)

print(performOp(43, lambda (int x) x * x))
```

### Closures

Lamba's can capture variables from the scope they are defined in.
```n
fun performOp(int val, function{int, int} op)
  op(val)

fun makeAdder(int y)
  lambda (int x) x + y

print(performOp(42, makeAdder(1337))
```

## Instance calls

The first argument to a function can optionally be placed in front of the function call.
This is syntactic sugar only, but can aid in making function 'chains' easier to read.
```n
fun isEven(int x) (x % 2) == 0
fun square(int x) x * x

print(rangeList(0, 100).filter(isEven).map(square))
```

## Parallel computing

Putting the keyword `fork` in front of any function call runs it on its own executor (thread) and
returns a `future{T}` handle to the executor.

```n
fun fib(int n)
  n <= 1 ? n : fib(n - 1) + fib(n - 2)

fun calc()
  a = fork fib(25);
  b = fork fib(26);
  a.get() + b.get()
```
Note this is where a pure functional language shines, its completely safe to run any pure function
in parallel without any need for synchronization.

## Actions
As mentioned before functions are pure and cannot have any side-effects, but a program without
side-effects is not really useful (something about a tree falling in a forest..).

That's why there are special kind of impure functions which are allowed to perform side-effects.
Those functions are called 'actions'. An action is allowed to call into a function but not vise
versa.

```n
import "std.nov"

act printFile(Path file)
  in  = fileOpen(file, FileMode.Open);
  out = consoleOpen(ConsoleKind.StdOut);
  copy(in, out)

act main()
  print("Which file do you want to print?");
  p = Path(readLine());
  print("Ok, printing: " + p);
  printFile(p)

main()
```

Note: To pass an action as a value you use the `action{}` type template instead of the
`function{}` one.

Note: To create an 'action' lambda you can use the `impure` keyword in front of the lambda:
`impure lambda (int x) x * x`

# Try it out

## Examples

Several examples can be found in the [examples](https://github.com/BastianBlokland/novus/tree/master/examples)
directory.

## Docker

You can quickly try it out using docker.
Open a interactive container with the compiler, runtime and examples installed:
`docker run --rm -it bastianblokland/novus sh`

Run an example:
`nove examples/fizzbuzz.nov`

## Installing the compiler and runtime

At this time there are no releases of binary files, however you can try out the binaries produced
by the [ci](https://github.com/BastianBlokland/novus/actions).

The best way is to build the compiler and runtime yourself, the process is documented below.

# Building

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
For more convenience you can add the `bin` directory to your `PATH`.

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
