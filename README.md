
# A compiler and interpreter for the MiniJava syntax: [![CMake on a single platform](https://github.com/schriner/miniCompEval/actions/workflows/cmake-single-platform.yml/badge.svg?branch=master)](https://github.com/schriner/miniCompEval/actions/workflows/cmake-single-platform.yml)

Syntax dervied from: https://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html

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

## Grammar
```
          Program    ::=   MainClass ( ClassDeclaration )* <EOF>  
        MainClass    ::=   "class" Identifier "{" "public" "static" "void" "main" "(" "String" "[" "]" Identifier ")" "{" Statement "}" "}"  
 ClassDeclaration    ::=   "class" Identifier ( "extends" Identifier )? "{" ( VarDeclaration )* ( MethodDeclaration )* "}"  
   VarDeclaration    ::=   Type Identifier ";"  
MethodDeclaration    ::=   "public" Type Identifier "(" ( Type Identifier ( "," Type Identifier )* )? ")" "{" ( VarDeclaration )* ( Statement )* "return" Expression ";" "}"  
             Type    ::=   "boolean" ( "[" "]" )* 
                      |    "int" ( "[" "]" )*
                      |    Identifier ( "[" "]" )*
                      |    "boolean"
                      |    "int"
                      |    Identifier
        Statement    ::=   "{" ( Statement )* "}"
                      |    "if" "(" Expression ")" Statement "else" Statement
                      |    "while" "(" Expression ")" Statement
                      |    "System.out.println" "(" Expression ")" ";"
                      |    Identifier "=" Expression ";"
                      |    Identifier "[" Expression "]" "=" Expression ";"
       Expression    ::=   Expression ( "&&" | "<" | "+" | "-" | "*" ) Expression
                      |    Expression "[" Expression "]"
                      |    Expression "." "length"
                      |    Expression "." Identifier "(" ( Expression ( "," Expression )* )? ")"
                      |    <INTEGER_LITERAL>
                      |    "true"
                      |    "false"
                      |    Identifier
                      |    "this"
                      |    "new" "int" "[" Expression "]"
                      |    "new" Identifier "(" ")"
                      |    "!" Expression
                      |    "(" Expression ")"
       Identifier    ::=    <IDENTIFIER>
```

### Type and Syntax Violation Reporting

Sample Output:

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

## CMAKE produces:
### [MiniJava_syntax.output](doc/MiniJava_syntax.output)
    
Grammar specification from bison statefully

### MiniJava_syntax.dot 

Used to create a tree representation of the grammar specification ([tree.png](doc/tree.png)).

### mjavac: Compiler to ARM 32 bit assembly 
From the build directory: 

    ```
    mjavac [-p | -o <output_file> | --stdout ] <inputfile>.java
    ```

### meval:  Interpreter
From the build directory: 

    ```
    meval [-o <output_file>] <inputfile>.java
    ```
Functionality is tested for the files contained within: `test/Interpreter`
* class method calls, recursion, and arrays are supported
* class variable delarations and keyword `extends` are unimplemented.

## Test Functionality
From within the build directory:
```
ctest
```
