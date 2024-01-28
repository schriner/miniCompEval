
cmake_minimum_required(VERSION 3.2)

project(mjavac)

find_package(BISON)
find_package(FLEX)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g --std=c++11")
set(AUTOGEN_FLEXBISON "${CMAKE_CURRENT_BINARY_DIR}/parse")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
file(MAKE_DIRECTORY ${AUTOGEN_FLEXBISON})

BISON_TARGET(MiniJava_syntax
	source/parser.y 
	${AUTOGEN_FLEXBISON}/MiniJava_syntax.cpp
	COMPILE_FLAGS
	"-v -d --graph=${AUTOGEN_FLEXBISON}/MiniJava_syntax.dot --debug -o"
)
FLEX_TARGET(MiniJava_lex source/parser.l ${AUTOGEN_FLEXBISON}/MiniJava_lex.cpp)
set(parser
	"${AUTOGEN_FLEXBISON}/MiniJava_syntax.cpp"
	"${AUTOGEN_FLEXBISON}/MiniJava_lex.cpp"
)

include_directories(include)
include_directories(${AUTOGEN_FLEXBISON})

set(mjavac_sources
	"TreeNode.cpp"
	"AssemNode.cpp"
	"util_eval.cpp"
	"util_assem.cpp"
)
list(TRANSFORM mjavac_sources PREPEND "source/")

add_executable(meval source/meval.cpp ${mjavac_sources} ${parser})

add_executable(mjavac source/main.cpp ${mjavac_sources} ${parser})
set_target_properties(mjavac PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DASSEM") 

enable_testing()

add_test(NAME ParseHanoiDemo COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/HanoiDemo.java)
add_test(NAME ParseLoop COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/loop.java)
add_test(NAME ParseArray COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/array.java)
add_test(NAME ParseNumber COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/number.java)
add_test(NAME ParseSimple COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/simple.java)

add_test(NAME ParseErrorVariableInMain COMMAND mjavac "-p" ${TEST_DIR}/ParserError/loopCheck.java)
set (passRegex "^${CMAKE_CURRENT_BINARY_DIR}/mjavac: syntax error at token: int\n${CMAKE_CURRENT_BINARY_DIR}/mjavac: ${TEST_DIR}/ParserError/loopCheck.java:5\n${CMAKE_CURRENT_BINARY_DIR}/:                 int a;\n")
set_property (TEST ParseErrorVariableInMain 
	              PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalSimple COMMAND meval ${TEST_DIR}/Interpreter/simple.java)
set (passRegex
	"This line should be printed. A bunch of expressions:\n5\n47\n-31\n75\n136\n"
)
set_property (TEST
	EvalSimple
	PROPERTY
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalMultiplePrint COMMAND meval ${TEST_DIR}/Compiler/multiplePrint.java)
set (passRegex
	"The answer\nto the meaning\nof life\nis:\n42\n"
)
set_property (TEST
	EvalMultiplePrint
	PROPERTY
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

if(PNG)
	add_custom_command(
		TARGET meval PRE_LINK
		COMMAND dot
		-Tpng
		-o
		"${AUTOGEN_FLEXBISON}/tree.png"
		"${AUTOGEN_FLEXBISON}/MiniJava_syntax.dot" 
	)
endif()
