
CC=clang++

BUILD=build_obj
PARSE=parse
INC=include
SRC=source

CCFlags=-I$(INC) -g --std=c++11
ASM=-D ASSEM

build: $(BUILD) $(PARSE) mjavac meval

$(BUILD):
	@mkdir $(BUILD)

$(PARSE):
	@mkdir $(PARSE)

$(PARSE)/MiniJava_syntax.cpp $(PARSE)/MiniJava_syntax.dot: $(PARSE) $(SRC)/parser.y $(INC)/TreeNode.hpp
	bison -v -y -d --graph=$(PARSE)/MiniJava_syntax.dot -t --debug -o $(PARSE)/MiniJava_syntax.cpp $(SRC)/parser.y

png: $(PARSE) $(PARSE)/MiniJava_syntax.dot
	dot -Tpng parse/MiniJava_syntax.dot > parse/tree.png

$(PARSE)/MiniJava_lex.cpp: $(SRC)/parser.l
	flex -o $(PARSE)/MiniJava_lex.cpp $(SRC)/parser.l

$(BUILD)/main.o $(BUILD)/util_assem.o: $(SRC)/main.cpp $(SRC)/util_assem.cpp $(BUILD)
	$(CC) $(CCFlags) -c $(ASM) -o $(BUILD)/util_assem.o $(SRC)/util_assem.cpp 
	$(CC) $(CCFlags) -c $(ASM) -o $(BUILD)/main.o $(SRC)/main.cpp 

$(BUILD)/meval.o $(BUILD)/util_eval.o: $(SRC)/meval.cpp $(SRC)/util_eval.cpp $(BUILD)
	$(CC) $(CCFlags) -c -o $(BUILD)/util_eval.o $(SRC)/util_eval.cpp 
	$(CC) $(CCFlags) -c -o $(BUILD)/meval.o $(SRC)/meval.cpp 

$(BUILD)/AssemTreeNode.o $(BUILD)/TreeNode.o: $(INC)/TreeNode.hpp $(SRC)/TreeNode.cpp $(BUILD)
	$(CC) $(CCFlags) -c -o $(BUILD)/TreeNode.o $(SRC)/TreeNode.cpp
	$(CC) $(CCFlags) -c $(ASM) -o $(BUILD)/AssemTreeNode.o $(SRC)/TreeNode.cpp

$(BUILD)/AssemNode.o: $(INC)/TreeNode.hpp $(SRC)/AssemNode.cpp $(BUILD)
	$(CC) $(CCFlags) -c $(ASM) -o $(BUILD)/AssemNode.o $(SRC)/AssemNode.cpp

$(BUILD)/AssemMiniJava_syntax.o $(BUILD)/MiniJava_syntax.o: $(PARSE)/MiniJava_syntax.cpp $(BUILD)
	$(CC) $(CCFlags) -c -o $(BUILD)/MiniJava_syntax.o $(PARSE)/MiniJava_syntax.cpp
	$(CC) $(CCFlags) -c -o $(BUILD)/AssemMiniJava_syntax.o $(PARSE)/MiniJava_syntax.cpp

$(BUILD)/AssemMiniJava_lex.o $(BUILD)/MiniJava_lex.o: $(PARSE)/MiniJava_lex.cpp $(BUILD)
	$(CC) $(CCFlags) -c -o $(BUILD)/MiniJava_lex.o $(PARSE)/MiniJava_lex.cpp 
	$(CC) $(CCFlags) -c $(ASM) -o $(BUILD)/AssemMiniJava_lex.o $(PARSE)/MiniJava_lex.cpp 

mjavac: $(BUILD)/AssemMiniJava_syntax.o $(BUILD)/AssemMiniJava_lex.o $(BUILD)/main.o $(BUILD)/AssemTreeNode.o $(BUILD)/AssemNode.o $(BUILD)/util_eval.o $(BUILD)/util_assem.o
	$(CC) $(CCFlags) $(ASM) -o mjavac $(BUILD)/main.o $(BUILD)/AssemTreeNode.o $(BUILD)/AssemNode.o $(BUILD)/util_eval.o $(BUILD)/util_assem.o $(BUILD)/AssemMiniJava_syntax.o $(BUILD)/AssemMiniJava_lex.o

meval: $(BUILD)/MiniJava_syntax.o $(BUILD)/MiniJava_lex.o $(BUILD)/meval.o $(BUILD)/TreeNode.o
	$(CC) $(CCFlags) -o meval $(BUILD)/meval.o $(BUILD)/util_eval.o $(BUILD)/TreeNode.o $(BUILD)/MiniJava_syntax.o $(BUILD)/MiniJava_lex.o

clean:
	rm -r mjavac meval $(PARSE) $(BUILD)
