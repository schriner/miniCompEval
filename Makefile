build: mjavac

MiniJava_syntax.cpp: parser.y TreeNode.hpp 
	bison -v -y -d --graph=MiniJava_syntax.dot -t --debug -o MiniJava_syntax.cpp parser.y

MiniJava_lex.cpp: parser.l
	flex -o MiniJava_lex.cpp parser.l

main.o: main.cpp
	clang++ -g -c -o main.o main.cpp 

TreeNode.o: TreeNode.hpp TreeNode.cpp
	clang++ -g -c -o TreeNode.o TreeNode.cpp

MiniJava_syntax.o: MiniJava_syntax.cpp 
	clang++ -g -c -o MiniJava_syntax.o MiniJava_syntax.cpp

MiniJava_lex.o: MiniJava_lex.cpp
	clang++ -g -c -o MiniJava_lex.o MiniJava_lex.cpp 

mjavac: MiniJava_syntax.o MiniJava_lex.o main.o TreeNode.o
	clang++ -g -o mjavac main.o TreeNode.o MiniJava_syntax.o MiniJava_lex.o

clean:
	rm mjavac main.o MiniJava_syntax.o MiniJava_lex.o MiniJava_syntax.cpp \
	MiniJava_lex.cpp TreeNode.o MiniJava_syntax.hpp MiniJava_syntax.dot MiniJava_syntax.output
