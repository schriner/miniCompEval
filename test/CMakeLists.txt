
enable_testing()

add_test(NAME ParseHanoiDemo COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/HanoiDemo.java)
add_test(NAME ParseLoop COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/loop.java)
add_test(NAME ParseArray COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/array.java)
add_test(NAME ParseNumber COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/number.java)
add_test(NAME ParseSimple COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/simple.java)
add_test(NAME ParseFor COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/loop3.java)
add_test(NAME ParseGarph COMMAND mjavac "-p" ${TEST_DIR}/Interpreter/graph.java)

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

add_test(NAME EvalLoop COMMAND meval ${TEST_DIR}/Interpreter/loop.java)
set (passRegex
	"Factorial of 10 is:3628800"
)
set_property (TEST EvalLoop 
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalNumber COMMAND meval ${TEST_DIR}/Interpreter/number.java)
set (passRegex
	"10 choose 4 is \n210\nGCD of 84, 132 is \n12\n"
)
set_property (TEST EvalNumber
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalHanoi COMMAND meval ${TEST_DIR}/Interpreter/HanoiDemo.java)
set (passRegex
	"move 1 from 1 to 3\nmove 2 from 1 to 2\nmove 1 from 3 to 2\nmove 3 from 1 to 3\nmove 1 from 2 to 1\nmove 2 from 2 to 3\nmove 1 from 1 to 3\nmove 4 from 1 to 2\nmove 1 from 3 to 2\nmove 2 from 3 to 1\nmove 1 from 2 to 1\nmove 3 from 3 to 2\nmove 1 from 1 to 3\nmove 2 from 1 to 2\nmove 1 from 3 to 2\nmove 5 from 1 to 3\nmove 1 from 2 to 1\nmove 2 from 2 to 3\nmove 1 from 1 to 3\nmove 3 from 2 to 1\nmove 1 from 3 to 2\nmove 2 from 3 to 1\nmove 1 from 2 to 1\nmove 4 from 2 to 3\nmove 1 from 1 to 3\nmove 2 from 1 to 2\nmove 1 from 3 to 2\nmove 3 from 1 to 3\nmove 1 from 2 to 1\nmove 2 from 2 to 3\nmove 1 from 1 to 3\n1"
)
set_property (TEST EvalHanoi
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalArray COMMAND meval ${TEST_DIR}/Interpreter/array.java)
set (passRegex "Sort successful\n") 
set_property (TEST EvalArray
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalArrayPrint COMMAND meval ${TEST_DIR}/Interpreter/array_print.java)
set (passRegex "^Printing Array of Length: 8\n60 40 20 0 50 70 10 30 \nPrinting Array of Length: 8\n0 10 20 30 40 50 60 70 \nSort successful\n") 
set_property (TEST EvalArrayPrint
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalMultiplePrint COMMAND meval ${TEST_DIR}/CodeGen/multiplePrint.java)
set (passRegex
  "The answer\nto the meaning\nof life\nis:\n42\n"
)
set_property (TEST
  EvalMultiplePrint
  PROPERTY
  PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalMultiDimArray COMMAND meval ${TEST_DIR}/Interpreter/multi_array.java)
set (passRegex
	"^Printing Array of Dim: 7 : 5\n: 0 1 2 3 4\n: 1 2 3 4 5\n: 2 3 4 5 6\n: 3 4 5 6 7\n: 4 5 6 7 8\n: 5 6 7 8 9\n: 6 7 8 9 10\nArray successful\n"
)
set_property (TEST
	EvalMultiDimArray
  PROPERTY
  PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalClassExtends COMMAND meval ${TEST_DIR}/Interpreter/extends.java)
set_property (TEST
	EvalClassExtends
  PROPERTY
  PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalArrayPrintClassVar COMMAND meval ${TEST_DIR}/Interpreter/array_print_class_var.java)
set (passRegex "^Printing Array of Length: 8\n60 40 20 0 50 70 10 30 \nPrinting Array of Length: 8\n0 10 20 30 40 50 60 70 \nSort successful\n") 
set_property (TEST EvalArrayPrintClassVar
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalNested COMMAND meval ${TEST_DIR}/Interpreter/nested.java)
set_property (TEST EvalNested
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalFactorial2 COMMAND meval ${TEST_DIR}/Interpreter/loop2.java)
set (passRegex "^Factorial of 10 is:3628800\nFactorial of 9 is:362880\nFactorial of 8 is:40320\nFactorial of 7 is:5040\nFactorial of 6 is:720\nFactorial of 5 is:120\nFactorial of 4 is:24\nFactorial of 3 is:6\nFactorial of 2 is:2\nFactorial of 1 is:1\nSuccess") 
set_property (TEST EvalFactorial2 
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalClassExtends2 COMMAND meval ${TEST_DIR}/Interpreter/extends2.java)
set_property (TEST
	EvalClassExtends2
  PROPERTY
  PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalFactorialFor COMMAND meval ${TEST_DIR}/Interpreter/loop3.java)
set_property (TEST EvalFactorialFor 
	PROPERTY 
	PASS_REGULAR_EXPRESSION "${passRegex}"
)

add_test(NAME EvalMultiplePrintCodeGen 
	COMMAND mjavac 
	${TEST_DIR}/CodeGen/multiplePrint.java
)

add_executable(test_llvm
	test_llvm.cpp
)
set (passRegex "Success\n")
add_test(NAME LLVM_Complex COMMAND test_llvm
	${TEST_DIR}/CodeGen/moreComplex.java
)
set_property(TEST LLVM_Complex
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_number COMMAND test_llvm
	${TEST_DIR}/CodeGen/number.java
)
set_property(TEST LLVM_number
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_HANOI COMMAND test_llvm
	${TEST_DIR}/Interpreter/HanoiDemo.java
)
set_property(TEST LLVM_HANOI
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_SimpleMethodCall COMMAND test_llvm
	${TEST_DIR}/CodeGen/simpleMethodCall.java
)
set_property(TEST LLVM_SimpleMethodCall
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_multiplePrintWAdd COMMAND test_llvm
	${TEST_DIR}/CodeGen/multiplePrintWAdd.java
)
set_property(TEST LLVM_multiplePrintWAdd
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_basicIf COMMAND test_llvm
	${TEST_DIR}/CodeGen/basicIf.java
)
set_property(TEST LLVM_basicIf
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_multiplePrintNumber COMMAND test_llvm
	${TEST_DIR}/CodeGen/multiplePrintNumber.java
)
set_property(TEST LLVM_multiplePrintNumber
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_loop COMMAND test_llvm
	${TEST_DIR}/CodeGen/loop.java
)
set_property(TEST LLVM_loop
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_simple COMMAND test_llvm
	${TEST_DIR}/CodeGen/simple.java
)
set_property(TEST LLVM_simple
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_simplefor COMMAND test_llvm
	${TEST_DIR}/CodeGen/simplefor.java
)
set_property(TEST LLVM_simplefor
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_singlePrint COMMAND test_llvm
	${TEST_DIR}/CodeGen/singlePrint.java
)
set_property(TEST LLVM_singlePrint
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
add_test(NAME LLVM_multiplePrint COMMAND test_llvm
	${TEST_DIR}/CodeGen/multiplePrint.java
)
set_property(TEST LLVM_multiplePrint
	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
)
#add_test(NAME LLVM_loop2 COMMAND test_llvm
#	${TEST_DIR}/CodeGen/loop2.java
#)
#set_property(TEST LLVM_loop2
#	PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}"
#)
