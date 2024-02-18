
/*
 * miniCompEval
 * Sonya Schriner
 * File: LLVM.cpp
 *
 */


//#include "TreeNode.hpp"

#include <system_error>

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm-c/BitWriter.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FileSystem.h>
#include "llvm/Support/raw_ostream.h"


using namespace std;
using namespace llvm;

extern ofstream bcStream;
extern string bcFilename;

// Emit Compilable LLVM IR
//void GenerateIR(Program * root) {
void GenerateIR() {
	// root->m
	// for (auto classdecl : root->c) {
	// }

	LLVMContext Context;

	// Create some module to put our function into it.
	Module *M = new Module("test", Context);

	// Create the main function: first create the type 'int ()'
	FunctionType *FT =
		FunctionType::get(Type::getInt32Ty(Context), /*not vararg*/false);

	// By passing a module as the last parameter to the Function constructor,
	// it automatically gets appended to the Module.
	Function *F = Function::Create(FT, Function::ExternalLinkage, "main", M);

	// Add a basic block to the function... again, it automatically inserts
	// because of the last argument.
	BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", F);

	// Get pointers to the constant integers...
	Value *Two = ConstantInt::get(Type::getInt32Ty(Context), 2);
	Value *Three = ConstantInt::get(Type::getInt32Ty(Context), 3);

	// Create the add instruction... does not insert...
	Instruction *Add = BinaryOperator::Create(Instruction::Add, Two, Three,
			"addresult");

	// explicitly insert it into the basic block...
	Add->insertInto(BB, BB->end());

	// Create the return instruction and add it to the basic block
	ReturnInst::Create(Context, Add)->insertInto(BB, BB->end());

	// Output the bitcode file to stdout
	std::error_code ec;
	ToolOutputFile bc(bcFilename.c_str(), ec, llvm::sys::fs::OF_None);
	//WriteBitcodeToFile(*M, bc.os());
	//WriteBitcodeToFile(*M, bcStream);
	//WriteBitcodeToFile(*M, outs());

  //M->print(outs(), nullptr, false, true); // method 1
  M->print(bc.os(), nullptr, false, true); // method 1
	bc.keep();


	// Delete the module and all of its contents.
	delete M;

}
