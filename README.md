
# A compiler and interpreter for the MiniJava syntax: [![CMake on a single platform](https://github.com/schriner/miniCompEval/actions/workflows/cmake-single-platform.yml/badge.svg?branch=master)](https://github.com/schriner/miniCompEval/actions/workflows/cmake-single-platform.yml)

## meval:  Interpreter
From the build directory: 

    ```
    meval [-o <output_file>] <inputfile>.java
    ```
Functionality is tested for the files contained within: [`test/Interpreter`](test/Interpreter)
* class method calls, variables, recursion with arguments, class variable delarations, and multi dimensional arrays are supported for bool and int types
* keyword `extends` and testing for recursion with `this` are unimplemented 

## mjavac: Compiler to ARM 32 bit assembly 
From the build directory: 

    ```
    mjavac [-p | -o <output_file> | --stdout ] <inputfile>.java
    ```
* (todo) functionality supported
* TODO: Automated testing to compile output files.
* Some sample output is located in [`test/CompilerOut`](test/CompilerOut)
To utilize the present test env: 
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

Error Output:

```
./build/mjavac: syntax error at token: int
./build/mjavac: test/ParserError/loopCheck.java:5
./build/mjavac:                 int a;
```
```
./build/meval: Type Violation in Line 6
./build/meval: test/Interpreter/simpleCheck.java:6
./build/meval:         if (true + (2 < 3)) {
```
```
./build/meval: Type Violation in Line 27
./build/meval: test/Interpreter/loopCheck.java:27
./build/meval:             multiplier = multiplier + 1;
```

## Build (MacOs)
### Dependencies:
* clang++
* flex
* bison

### CMake:
In the root directory
```
cmake -B build
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

## Test Functionality
From within the build directory:
```
ctest
```

Output should mirror output from `javac`
