
## Grammar
```
                 Program    ::=   MainClass ( ClassDeclaration )* <EOF>  
               MainClass    ::=   "class" Identifier "{" "public" "static" "void" "main" "(" "String" "[" "]" Identifier ")" "{" Statement "}" "}"  
        ClassDeclaration    ::=   "class" Identifier ( "extends" Identifier )? "{" ( VarDeclaration )* ( MethodDeclaration )* "}"  
          VarDeclaration    ::=   Type Identifier ";"  
VarDeclarationExpression    ::=   Type Identifier "=" Expression 
       MethodDeclaration    ::=   "public" Type Identifier "(" ( Type Identifier ( "," Type Identifier )* )? ")" "{" ( VarDeclaration )* ( Statement )* "return" Expression ";" "}"  
                    Type    ::=   "boolean" ( "[" "]" )* 
                            |    "int" ( "[" "]" )*
                            |    Identifier ( "[" "]" )*
                            |    "boolean"
                            |    "int"
                            |    Identifier
               Statement    ::=   "{" ( VarDeclarationExpression ";" | VarDeclaration )* ( Statement )* "}"
                            |    "if" "(" Expression ")" Statement "else" Statement
                            |    "while" "(" Expression ")" Statement
                            |    "for" "(" VarDeclarationExpression ";" Expression ";" Identifier "=" Expression ")" Statement
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

