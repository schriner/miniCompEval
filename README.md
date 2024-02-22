
# A Code Generator and interpreter for the MiniJava syntax: [![CMake on a single platform](https://github.com/schriner/miniCompEval/actions/workflows/cmake-single-platform.yml/badge.svg?branch=master)](https://github.com/schriner/miniCompEval/actions/workflows/cmake-single-platform.yml)

## meval:  Interpreter
From the build directory: 

```
meval [-o <output_file>] <inputfile>.java
```
Functionality is tested for the files contained within: [`test/Interpreter`](test/Interpreter)
* class method calls, variables, recursion with arguments, class variable delarations, keyword `extends`, multi dimensional arrays, `if`/`while`/`for` control flow, and variable declarations within `{` `}` statements

## mjavacllvm:  Generator for LLVM Bitcode
From the build directory: 

```
mjavacllvm [-p | -o <output_file> | --stdout ] <inputfile>.java
```
Functionality is tested for the files contained within:
* Some sample LLVM IR output is located in [`test/LLVM_IR`](test/LLVM_IR)
* `test/test_llvm.sh <input/path>` runs a diff between generated IR and javac output
* Test via compiling bitcode and comparing output with `java`
* Supported: `if`/`while`, method calls with integer/boolean arguments, integer/boolean variable declarations, and recursion with integer/boolean arguments
* TODO: `for`, Arrays, classes, class extensions, ID objects
* TODO: Any optimization to IR

Generated IR is compileable with `clang`

## mjavac:  Code Generator for ARM 32 bit assembly 
From the build directory: 

```
mjavac [-p | -o <output_file> | --stdout ] <inputfile>.java
```
* Supported functionality would include object method calls, expression evaluation, expression assignment to variables, print() via prtinf, and control flow statements
* TODO: Symbol table improvements, arrays, class instance variables, `this`, nested method calls, class extends
* Some sample `.s` output is located in [`test/Armv7Out`](test/Armv7Out)

To utilize the present test compilation env: 
```
<within the root dir>
docker build -t mjavac_arm_test .
docker run -it --rm mjavac_arm_test
cd CompilerOut
make
```

## Grammar : 
see for Grammar specification:
* [readme](doc/readme.md)
* [MiniJava_syntax.output](doc/MiniJava_syntax.output)
* Tree representation: ([tree.png](doc/tree.png))

Syntax dervied from: https://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html

### Type and Syntax Violation Reporting

* TODO: type checking for variable assignments and variable name redundancy

Error Output:

```
./build/mjavac: syntax error at token: int
./build/mjavac: test/ParserError/loopCheck.java:5
./build/mjavac:                 int a;
```
```
./build/mjavac: Type Violation in Line at token: "true" with expr "+", lineno:6
./build/mjavac: test/TypeError/simpleCheck.java:6
./build/mjavac:         if (true + (2 < 3)) {
[1]    61376 abort      ./build/mjavac -p test/TypeError/simpleCheck.java
```
```
./build/mjavac: Type Violation in Line at token: ")" with while conditional, lineno:25
./build/mjavac: test/TypeError/loopCheck.java:25
./build/mjavac:         while (multiplier + 10) {
[1]    4590 abort      ./build/mjavac -p test/TypeError/loopCheck.java
```

## Build (MacOs)
### Dependencies:
* clang++
* flex
* bison
* LLVM 17

### CMake:
In the root directory
```
cmake -DCMAKE_C_COMPILER=<path/to/clang> \
    -DCMAKE_CXX_COMPILER=<path/to/clang++> \
    -DLLVM_DIR=<path/to/llvm17> \
    -B build
cmake --build build
```

Corresponding executables will be placed in `build`

```
cmake -DPNG=ON -B build
cmake --build build
```
Generates a PNG of the tree during the build process as well. The tree can be found in `build/parse/tree.png`.

### Makefile:
In the root directory

```
make
```

Create Tree PNG:

```
make png
```

## Automated Testing for Parser, Interpreter, and LLVM IR
From within the build directory:
```
ctest
```
## Automated Testing for Assembly
From within the root directory:
```
./test/test_assem.sh
```
Produces output like:
```
././test/../build/mjavac -o ./test/Armv7Out/basicIf.s ./test/Armv7Out/basicIf.java
././test/../build/mjavac -o ./test/Armv7Out/loop.s ./test/Armv7Out/loop.java

...

Beginning Test: simpleMethodCall
Success

...
```

Output should mirror output from `javac`. Automated compilation testing for correct execution of .s files 

