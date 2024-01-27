
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

add_test(NAME ${TEST_DIR}/ParseHanoiDemo COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/HanoiDemo.java)
add_test(NAME ${TEST_DIR}/ParseLoop COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/loop.java)
add_test(NAME ${TEST_DIR}/ParseArray COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/array.java)
add_test(NAME ${TEST_DIR}/ParseNumber COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/number.java)
add_test(NAME ${TEST_DIR}/ParseSimple COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/simple.java)
#add_test(NAME ${TEST_DIR}/VariableInMain COMMAND mjavac "-p" ${TEST_DIR}/ParserError/loopCheck.java)

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
