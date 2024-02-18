
/*
 * miniCompEval
 * Sonya Schriner
 * File: LLVM.cpp
 *
 */


#include "TreeNode.hpp"

#include <system_error>

#include "llvm/ADT/StringRef.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm-c/BitWriter.h"
#include "llvm-c/Types.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FileSystem.h>
#include "llvm/Support/raw_ostream.h"


using namespace std;

extern string bcFilename;
llvm::Function *_printf;

llvm::Instruction * buildExpression(Exp * exp, llvm::LLVMContext &Context, llvm::BasicBlock *BB) {
	
	if (Or* e = dynamic_cast<Or * >(exp)) {
	} else if (And* e = dynamic_cast<And * >(exp)) {
	} else if (Equal* e = dynamic_cast<Equal * >(exp)) {
	} else if (NotEqual* e = dynamic_cast<NotEqual * >(exp)) {
	} else if (Lesser* e = dynamic_cast<Lesser * >(exp)) {
	} else if (Greater* e = dynamic_cast<Greater * >(exp)) {
	} else if (LessEqual* e = dynamic_cast<LessEqual * >(exp)) {
	} else if (GreatEqual* e = dynamic_cast<GreatEqual * >(exp)) {
	} else if (Add* e = dynamic_cast<Add * >(exp)) {
	} else if (Subtract* e = dynamic_cast<Subtract * >(exp)) {
	} else if (Divide* e = dynamic_cast<Divide * >(exp)) {
	} else if (Multiply* e = dynamic_cast<Multiply * >(exp)) {
	} else if (Not* e = dynamic_cast<Not * >(exp)) {
	} else if (Pos* e = dynamic_cast<Pos * >(exp)) {
	} else if (Neg* e = dynamic_cast<Neg * >(exp)) {
	//} else if (ExpObject* e = dynamic_cast< * >(exp)) {
	//} else if (ArrayAccess* e = dynamic_cast< * >(exp)) {
	//} else if (Length* e = dynamic_cast< * >(exp)) {
	//} else if (ArrayAccessLength* e = dynamic_cast< * >(exp)) {
	} else if (LitInt* e = dynamic_cast<LitInt * >(exp)) {
	} else if (True* e = dynamic_cast<True * >(exp)) {
	} else if (False* e = dynamic_cast<False * >(exp)) {
	//} else if (ExpObject* e = dynamic_cast< * >(exp)) {
	//} else if (ObjectMethodCall* e = dynamic_cast< * >(exp)) {
	} else if (ParenExp* e = dynamic_cast<ParenExp * >(exp)) {
	} else {
		cerr << "Error processing buildExpression: " << endl;
		cerr << "Expresion e cast error " << endl;
		abort();
	}

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

llvm::Instruction * buildStatement(Statement *s, llvm::LLVMContext &Context, llvm::BasicBlock *BB) {
	
	// TODO: System.out.println
	// TODO: System.out.print
	// TODO: llvm::Value string constant

	if (BlockStatements * block_s = dynamic_cast<BlockStatements * >(s)) {
		// TODO: VarDeclExpList * vdel = nullptr;
		// TODO: StatementList * s = nullptr;
		llvm::Instruction * i = nullptr;
		if (s) {
			for (auto state : *block_s->s->sVector ) {
				i = buildStatement(state, Context, BB);
			}
		}
		return i;
	//} else if (IfStatement * if_s = dynamic_cast<IfStatement * >(s)) {
	//	buildExpression(if_s->e, Context, BB);
	//	buildStatement(if_s->s_if, Context, BB);
	//	buildStatement(if_s->s_el, Context, BB);
	//	cerr << "IfStatement: unimplemented" << endl;
	//} else if (WhileStatement * while_s = dynamic_cast<WhileStatement * >(s)) {
	//	cerr << "WhileStatement: unimplemented" << endl;
	//} else if (ForStatement * for_s = dynamic_cast<ForStatement * >(s)) {
	//	cerr << "ForStatement: unimplemented" << endl;
	//} else if (PrintExp * p_exp = dynamic_cast<PrintExp * >(s)) {
		//buildExpression();
	//	cerr << "PrintExp: unimplemented" << endl;
	} else if (PrintString * p_str = dynamic_cast<PrintString * >(s)) {
		llvm::Constant * StrConstant = llvm::ConstantDataArray::getString(
				Context, p_str->s->str->c_str()
		);
		auto *GV = new llvm::GlobalVariable(
				*BB->getParent()->getParent(), StrConstant->getType(), true, llvm::GlobalValue::PrivateLinkage,
				StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
		GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		GV->setAlignment(llvm::Align(1));
		llvm::CallInst *CallPrint = llvm::CallInst::Create(_printf, GV, "printf", BB);
		return CallPrint;
	//} else if (PrintLineExp * pln_exp = dynamic_cast<PrintLineExp * >(s)) {
		//buildExpression();
	//	cerr << "PrintLineExp: unimplemented" << endl;
	} else if (PrintLineString * pln_str = dynamic_cast<PrintLineString * >(s)) {
		llvm::Constant *StrConstant = llvm::ConstantDataArray::getString(Context, *pln_str->s->str + "\n");
		auto *GV = new llvm::GlobalVariable(
				*BB->getParent()->getParent(), StrConstant->getType(), true, llvm::GlobalValue::PrivateLinkage,
				StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
		GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		GV->setAlignment(llvm::Align(1));
		llvm::CallInst *CallPrint = llvm::CallInst::Create(_printf, GV, "printf", BB);
		return CallPrint;
	//} else if (Assign * assign = dynamic_cast<Assign * >(s)) {
	//	cerr << "Assign: unimplemented" << endl;
	//} else if (IndexAssign * idx = dynamic_cast<IndexAssign * >(s)) {
	//	cerr << "IndexAssign: unimplemented" << endl;
	//} else if (ReturnStatement * ret_s = dynamic_cast<ReturnStatement * >(s)) {
	//	cerr << "ReturnStatement: unimplemented" << endl;
	} else {
		cerr << "Error processing buildStatement: " << endl;
		cerr << "Statement s cast error " << endl;
		abort();
	}
	
	return nullptr;

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

	// Create the main function: first create the type 'int ()'
	llvm::FunctionType *FT_printf =
		llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), { llvm::PointerType::get(llvm::Type::getInt8Ty(Context), 0)}, true);

	// By passing a module as the last parameter to the Function constructor,
	// it automatically gets appended to the Module.
	_printf = llvm::Function::Create(FT_printf, llvm::Function::ExternalLinkage, "printf", M);
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

