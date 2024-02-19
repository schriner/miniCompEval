
cmake_minimum_required(VERSION 3.5)

project(mjavac)

find_package(BISON REQUIRED EXACT 3.8.2)
find_package(FLEX REQUIRED EXACT 2.6.4)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g --std=c++17")
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
	"Evaluate.cpp"
	"TreeNode.cpp"
	"ARMv7.cpp"
)
list(TRANSFORM mjavac_sources PREPEND "source/")

add_executable(meval source/meval.cpp ${mjavac_sources} ${parser})

add_executable(mjavac source/main.cpp ${mjavac_sources} ${parser})
set_target_properties(mjavac PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DASSEM") 

message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
find_package(LLVM CONFIG REQUIRED EXACT 17.0.6)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
if(LLVM_LINK_LLVM_DYLIB)
	set(llvm_libs LLVM)
else()
	message(STATUS "llvm_map_components_to_libnames")
	llvm_map_components_to_libnames(llvm_libs
		support core irreader
		bitreader bitwriter
		passes asmprinter
		aarch64info aarch64desc aarch64codegen aarch64asmparser
		armcodegen armasmparser
		interpreter mcjit
		nvptxdesc
		x86info x86codegen x86asmparser
		sparccodegen sparcasmparser
		webassemblydesc)
endif()
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

add_subdirectory(${TEST_DIR})
add_executable(mjavacllvm source/mjavacllvm.cpp source/LLVM.cpp source/Evaluate.cpp source/TreeNode.cpp ${parser})
target_link_libraries(mjavacllvm ${llvm_libs})

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
