
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

// TODO: turn this into a state struct ?
extern string bcFilename;
llvm::Function *_printf;
llvm::GlobalVariable *_exp_format;
llvm::GlobalVariable *_exp_format_n;

map<string, llvm::Function *> func_table;

llvm::Value * buildExpression(Exp * exp, llvm::LLVMContext &Context, llvm::BasicBlock *BB) {
	llvm::Instruction *i;

	if (Or* e = dynamic_cast<Or * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		i = llvm::BinaryOperator::Create(llvm::Instruction::Or, lhs, rhs, "");

	} else if (And* e = dynamic_cast<And * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		i = llvm::BinaryOperator::Create(llvm::Instruction::And, lhs, rhs, "");

	} else if (Equal* e = dynamic_cast<Equal * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		return new llvm::ICmpInst(*BB, llvm::CmpInst::ICMP_EQ, lhs, rhs, "");

	} else if (NotEqual* e = dynamic_cast<NotEqual * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		return new llvm::ICmpInst(*BB, llvm::CmpInst::ICMP_NE, lhs, rhs, "");

	} else if (Lesser* e = dynamic_cast<Lesser * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		return new llvm::ICmpInst(*BB, llvm::CmpInst::ICMP_SLT, lhs, rhs, "");

	} else if (Greater* e = dynamic_cast<Greater * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		return new llvm::ICmpInst(*BB, llvm::CmpInst::ICMP_SGT, lhs, rhs, "");

	} else if (LessEqual* e = dynamic_cast<LessEqual * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		return new llvm::ICmpInst(*BB, llvm::CmpInst::ICMP_SLE, lhs, rhs, "");

	} else if (GreatEqual* e = dynamic_cast<GreatEqual * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		return new llvm::ICmpInst(*BB, llvm::CmpInst::ICMP_SGE, lhs, rhs, "");

	} else if (Add* e = dynamic_cast<Add * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		i = llvm::BinaryOperator::Create(llvm::Instruction::Add, lhs, rhs, "");

	} else if (Subtract* e = dynamic_cast<Subtract * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		i = llvm::BinaryOperator::Create(
			llvm::Instruction::Sub, lhs, rhs, ""
		);

	} else if (Divide* e = dynamic_cast<Divide * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		i = llvm::BinaryOperator::Create(
			llvm::Instruction::SDiv, lhs, rhs, ""
		);

	} else if (Multiply* e = dynamic_cast<Multiply * >(exp)) {
		llvm::Value *lhs = buildExpression(e->e1, Context, BB); 
		llvm::Value *rhs = buildExpression(e->e2, Context, BB);
		i = llvm::BinaryOperator::Create(
			llvm::Instruction::Mul, lhs, rhs, ""
		);

	} else if (Not* e = dynamic_cast<Not * >(exp)) {
		llvm::Value *exp = buildExpression(e->e, Context, BB);
		// invert operand or constant

		//i = llvm::UnaryOperator::Create(
		//	llvm::Instruction::Not, exp, ""
		//);

	} else if (Pos* e = dynamic_cast<Pos * >(exp)) {
		return buildExpression(e->e, Context, BB);

	} else if (Neg* e = dynamic_cast<Neg * >(exp)) {
		llvm::Value *rhs = buildExpression(e->e, Context, BB);
		i = llvm::BinaryOperator::Create(
			llvm::Instruction::Mul,
			llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), -1), 
			rhs, ""
		);

	//} else if (ExpObject* e = dynamic_cast< * >(exp)) {
	//} else if (ArrayAccess* e = dynamic_cast< * >(exp)) {
	//} else if (Length* e = dynamic_cast< * >(exp)) {
	//} else if (ArrayAccessLength* e = dynamic_cast< * >(exp)) {
	} else if (LitInt* e = dynamic_cast<LitInt * >(exp)) {
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), e->i);

	} else if (True* e = dynamic_cast<True * >(exp)) {
		return llvm::ConstantInt::getTrue(Context);

	} else if (False* e = dynamic_cast<False * >(exp)) {
		return llvm::ConstantInt::getFalse(Context);

	} else if (ExpObject* e = dynamic_cast<ExpObject * >(exp)) {
		cerr << "Error processing buildExpression: " << endl;
		cerr << "ExpObject unimplemented " << endl;
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 1);

	} else if (ObjectMethodCall* e = dynamic_cast<ObjectMethodCall * >(exp)) {
		//return llvm::CallInst::Create(func_table[*e->o->id+*e->i->id], "", BB);
		// TODO classname
		return llvm::CallInst::Create(func_table[*e->i->id], "", BB);

	} else if (ParenExp* e = dynamic_cast<ParenExp * >(exp)) {
		return buildExpression(e->e, Context, BB);

	} else {
		cerr << "Error processing buildExpression: " << endl;
		cerr << "Expresion e cast error " << endl;
		abort();
	}
	
	i->insertInto(BB, BB->end());
	return i;

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

	} else if (IfStatement * if_s = dynamic_cast<IfStatement * >(s)) {
		llvm::Value * cond = buildExpression(if_s->e, Context, BB);
		llvm::BasicBlock * ifBB = 
			llvm::BasicBlock::Create(Context, "if", BB->getParent());
		llvm::BasicBlock * elBB = 
			llvm::BasicBlock::Create(Context, "el", BB->getParent());
		buildStatement(if_s->s_if, Context, ifBB);
		buildStatement(if_s->s_el, Context, elBB);
		llvm::BranchInst::Create(ifBB, elBB, cond, BB);
		llvm::BasicBlock * succBB = 
			llvm::BasicBlock::Create(Context, "s", BB->getParent());
		llvm::BranchInst::Create(succBB, ifBB);
		llvm::BranchInst::Create(succBB, elBB);
		return &succBB->back();

	} else if (WhileStatement * while_s = dynamic_cast<WhileStatement * >(s)) {
		// FIXME Cond Block
		// While Block
		// After Block
		llvm::Value * cond = buildExpression(while_s->e, Context, BB);
		llvm::BasicBlock * whileBB = 
			llvm::BasicBlock::Create(Context, "w", BB->getParent());
		llvm::BasicBlock * succBB = 
			llvm::BasicBlock::Create(Context, "s", BB->getParent());
		
		buildStatement(while_s->s, Context, whileBB);
		llvm::BranchInst::Create(whileBB, succBB, cond, BB);

		cond = buildExpression(while_s->e, Context, BB);
		llvm::BranchInst::Create(whileBB, succBB, cond, whileBB);
		return &succBB->back();

	//} else if (ForStatement * for_s = dynamic_cast<ForStatement * >(s)) {
	//	cerr << "ForStatement: unimplemented" << endl;

	} else if (PrintExp * p_exp = dynamic_cast<PrintExp * >(s)) {
		llvm::CallInst *CallPrint = llvm::CallInst::Create(
			_printf, {_exp_format, buildExpression(p_exp->e, Context, BB)},
			"", BB
		);
		return CallPrint;

	} else if (PrintString * p_str = dynamic_cast<PrintString * >(s)) {
		llvm::Constant * StrConstant = llvm::ConstantDataArray::getString(
				Context, p_str->s->str->c_str()
		);
		auto *GV = new llvm::GlobalVariable(
				*BB->getParent()->getParent(), StrConstant->getType(), true, llvm::GlobalValue::PrivateLinkage,
				StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
		GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		GV->setAlignment(llvm::Align(1));
		llvm::CallInst *CallPrint = llvm::CallInst::Create(
			_printf, GV, "", BB
		);
		return CallPrint;

	} else if (PrintLineExp * pln_exp = dynamic_cast<PrintLineExp * >(s)) {
		llvm::CallInst *CallPrint = llvm::CallInst::Create(
			_printf,{_exp_format_n, buildExpression(pln_exp->e, Context, BB)},
			"", BB
		);
		return CallPrint;

	} else if (PrintLineString * pln_str = dynamic_cast<PrintLineString * >(s)) {
		llvm::Constant *StrConstant = llvm::ConstantDataArray::getString(Context, *pln_str->s->str + "\n");
		auto *GV = new llvm::GlobalVariable(
				*BB->getParent()->getParent(), StrConstant->getType(), true, llvm::GlobalValue::PrivateLinkage,
				StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
		GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		GV->setAlignment(llvm::Align(1));
		llvm::CallInst *CallPrint = llvm::CallInst::Create(
			_printf, GV, "", BB
		);
		return CallPrint;
		
	} else if (Assign * assign = dynamic_cast<Assign * >(s)) {
		cerr << "Assign: unimplemented" << endl;
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
	//llvm::FunctionType * ClassType =
	//	llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);
	//llvm::Function *Class =
  //   llvm::Function::Create(FT, llvm::Function::ExternalLinkage, *c->i->id, M);

	// VarDeclList * v = nullptr;

	for (auto func : *c->m->mdVector) {
		// TODO(ss) FIXME return and arg type

		// VarDeclList * v = nullptr;
		if (VarDeclList * v = func->v) {\
			for (auto var : *v->vdVector ) {
				cerr << *var->i->id << endl;
			}
		}

		llvm::FunctionType *FT =
			llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);
		llvm::Function *F =
			llvm::Function::Create(
					FT, llvm::Function::ExternalLinkage, *c->i->id + *func->i->id, M
		);

		//func_table[*c->i->id+*func->i->id] = F;
		func_table[*func->i->id] = F;
		
		llvm::BasicBlock *BB = 
			llvm::BasicBlock::Create(Context, "EntryBlock", F);
		
		if (func->s) {
			for (auto state : *func->s->sVector ) {
				buildStatement(state, Context, BB);
			}
		}

		llvm::ReturnInst::Create(Context, 
			buildExpression(func->e, Context, BB), 
			&F->back());
	}

}

void buildMain(MainClass * main, llvm::LLVMContext &Context, llvm::Module *M) {
	// Create the main function: first create the type 'int ()'
	llvm::FunctionType *FT =
		llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), false);

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
	llvm::ReturnInst::Create(Context, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0), &F->back());

}

// Emit Compilable LLVM IR
void GenerateIR(Program * root) {
	llvm::LLVMContext Context;

	// Create some module to put our function into it.
	llvm::Module *M = new llvm::Module(*root->m->i1->id, Context);

	llvm::Constant * StrConstant = 
		llvm::ConstantDataArray::getString(Context, "%d");
	_exp_format = new llvm::GlobalVariable(
		*M, StrConstant->getType(), true, llvm::GlobalValue::PrivateLinkage,
		StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
	_exp_format->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
	_exp_format->setAlignment(llvm::Align(1));

	StrConstant = llvm::ConstantDataArray::getString(Context, "%d\n");
	_exp_format_n = new llvm::GlobalVariable(
		*M, StrConstant->getType(), true, llvm::GlobalValue::PrivateLinkage,
		StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
	_exp_format_n->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
	_exp_format_n->setAlignment(llvm::Align(1));

	// Create the main function: first create the type 'int ()'
	llvm::FunctionType *FT_printf =
		llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), { llvm::PointerType::get(llvm::Type::getInt8Ty(Context), 0)}, true);

	_printf = llvm::Function::Create(
		FT_printf, llvm::Function::ExternalLinkage, "printf", M
	);

	if (root->c) {
		for (auto classdecl : *root->c->cdVector) {
			buildClassDecl(classdecl, Context, M);
		}
	}
	buildMain(root->m, Context, M);

	// Output the bitcode file to stdout
	// TODO(ss): print target triple for computer arch to file
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

