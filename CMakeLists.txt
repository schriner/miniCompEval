
cmake_minimum_required(VERSION 3.5)

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
	"EvalNode.cpp"
	"TreeNode.cpp"
	"AssemNode.cpp"
	"util_eval.cpp"
	"util_assem.cpp"
)
list(TRANSFORM mjavac_sources PREPEND "source/")

add_executable(meval source/meval.cpp ${mjavac_sources} ${parser})

add_executable(mjavac source/main.cpp ${mjavac_sources} ${parser})
set_target_properties(mjavac PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DASSEM") 

add_subdirectory(${TEST_DIR})

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
