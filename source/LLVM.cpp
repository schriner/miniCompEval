
/*
 * miniCompEval
 * Sonya Schriner
 * File: LLVM.cpp
 *
 */


#include "TreeNode.hpp"

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

extern string bcFilename;

void buildExpression() {

}

llvm::Instruction * buildStatement(Statement *s, llvm::LLVMContext &Context, llvm::BasicBlock *BB) {

	// TODO: System.out.println
	// TODO: System.out.print
	// TODO: llvm::Value string constant

	// Get pointers to the constant integers...
	llvm::Value *Two = 
		llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 2);
	llvm::Value *Three = 
		llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 3);

	// Create the add instruction... does not insert...
	llvm::Instruction *Add = 
		llvm::BinaryOperator::Create(llvm::Instruction::Add, Two, Three,
			"addresult");

	// explicitly insert it into the basic block...
	Add->insertInto(BB, BB->end());

	return Add;

}

void buildClassDecl(ClassDecl * c, llvm::LLVMContext &Context, llvm::Module *M) {
	// Create the main function: first create the type 'int ()'
	llvm::FunctionType *FT =
		llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), /*not vararg*/false);

	// By passing a module as the last parameter to the Function constructor,
	// it automatically gets appended to the Module.
	llvm::Function *F =
     llvm::Function::Create(FT, llvm::Function::ExternalLinkage, *c->i->id, M);
	// VarDeclList * v = nullptr;

	// MethodDeclList * m = nullptr;
	// buildMethod(c->m)	
}

void buildMain(MainClass * main, llvm::LLVMContext &Context, llvm::Module *M) {
	// Create the main function: first create the type 'int ()'
	llvm::FunctionType *FT =
		llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), /*not vararg*/false);

	// By passing a module as the last parameter to the Function constructor,
	// it automatically gets appended to the Module.
	llvm::Function *F = 
		llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", M);

	// Add a basic block to the function... again, it automatically inserts
	// because of the last argument.
	llvm::BasicBlock *BB = 
		llvm::BasicBlock::Create(Context, "EntryBlock", F);

	llvm::Instruction * last = buildStatement(main->s, Context, BB);

	// Create the return instruction and add it to the basic block
	llvm::ReturnInst::Create(Context, last)->insertInto(BB, BB->end());

}

// Emit Compilable LLVM IR
void GenerateIR(Program * root) {
	llvm::LLVMContext Context;

	// Create some module to put our function into it.
	llvm::Module *M = new llvm::Module(*root->m->i1->id, Context);

	buildMain(root->m, Context, M);
	if (root->c) {
		for (auto classdecl : *root->c->cdVector) {
			buildClassDecl(classdecl, Context, M);
		}
	}

	// Output the bitcode file to stdout
	std::error_code ec;
	llvm::ToolOutputFile bc(bcFilename.c_str(), ec, llvm::sys::fs::OF_None);
	//WriteBitcodeToFile(*M, bc.os());
	//WriteBitcodeToFile(*M, outs());
	

	if (bcFilename.empty()) {
  	M->print(llvm::outs(), nullptr, false, true); // method 1
  } else { 
		M->print(bc.os(), nullptr, false, true); // method 1
		bc.keep();
	}

	// Delete the module and all of its contents.
	delete M;

}

