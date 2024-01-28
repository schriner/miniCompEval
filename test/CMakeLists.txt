
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
set_property (TEST EvalSimple 
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

add_test(NAME EvalMultiplePrintCompiler COMMAND mjavac ${TEST_DIR}/Compiler/multiplePrint.java)