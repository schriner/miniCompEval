
/*
 * miniCompEval
 * Sonya Schriner
 * File: LLVM.cpp
 * Naive IR generation with no control flow analysis.
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
struct _SYMBOL {
	llvm::Value * v;
	Type * t;
};

struct _BLOCK_SCOPE {
	//map<string, llvm::Value *> stack;
	map<string, _SYMBOL> stack;
	_BLOCK_SCOPE * parent;
	const typename map<string, _SYMBOL>::iterator end() {
		return stack.end();
	}
	typename map<string, _SYMBOL>::iterator find(string id) {
		_BLOCK_SCOPE * s = this;
		while (s) {
			if (s->stack.find(id) != s->stack.end()) {
				return s->stack.find(id);
			}
			s = s->parent;
		}
		return end();
	}
	Type * get_type(string id) {
		if (auto it = find(id); it != end()) {
			return it->second.t;
		}
		return nullptr;
	}
	llvm::Value * operator[](const string id) {
		return find(id)->second.v;
	}
	void insert(const string id,  llvm::Value * v, Type * t) {
		stack[id] = {v, t};
	}
	_BLOCK_SCOPE(_BLOCK_SCOPE * p)
		: parent(p) {}
	_BLOCK_SCOPE()
		: parent(nullptr){}
	_BLOCK_SCOPE * push_back() { 
		return new _BLOCK_SCOPE(parent); 
	}
  _BLOCK_SCOPE * pop_back() { 
		if (parent == nullptr) {
			stack.clear();
			return this; 
		}
		auto tmp = parent;
		delete this;
		return tmp;
	}
};
_BLOCK_SCOPE * block_scope;
string * _this;

llvm::Value *
buildExpression(Exp * exp, llvm::LLVMContext &Context, llvm::BasicBlock *BB) {
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
		// This op should only be for bool values - semantic analysis
		llvm::Value *exp = buildExpression(e->e, Context, BB);
		return llvm::BinaryOperator::CreateNot(exp, "", BB);

	} else if (Pos* e = dynamic_cast<Pos * >(exp)) {
		return buildExpression(e->e, Context, BB);

	} else if (Neg* e = dynamic_cast<Neg * >(exp)) {
		llvm::Value *rhs = buildExpression(e->e, Context, BB);
		i = llvm::BinaryOperator::Create(
			llvm::Instruction::Mul,
			llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), -1), 
			rhs, ""
		);

	// TODO: Array
	//} else if (ArrayAccess* e = dynamic_cast< * >(exp)) {

	// TODO: Array
	//} else if (Length* e = dynamic_cast< * >(exp)) {

	// TODO: Array
	//} else if (ArrayAccessLength* e = dynamic_cast< * >(exp)) {

	} else if (LitInt* e = dynamic_cast<LitInt * >(exp)) {
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), e->i);

	} else if (True* e = dynamic_cast<True * >(exp)) {
		return llvm::ConstantInt::getTrue(Context);

	} else if (False* e = dynamic_cast<False * >(exp)) {
		return llvm::ConstantInt::getFalse(Context);

	} else if (ExpObject* e = dynamic_cast<ExpObject * >(exp)) {
		if (IdObj * id_obj = dynamic_cast<IdObj * >(e->o)) {
			if (dynamic_cast<IntType *>(block_scope->get_type(*id_obj->i->id))) {
				return new llvm::LoadInst(
						llvm::Type::getInt32Ty(Context),
						(*block_scope)[*id_obj->i->id], "", BB);

			} else if (dynamic_cast<BoolType *>(block_scope->get_type(*id_obj->i->id))) {
				return new llvm::LoadInst(
						llvm::Type::getInt1Ty(Context),
						(*block_scope)[*id_obj->i->id], "", BB);

			} else {
				// FIXME TYPES
				cerr << "ExpObject type unimplemented " << endl;
				abort();
			}

		} else if (ThisObj * t = dynamic_cast<ThisObj * >(e->o)) {
			return new llvm::LoadInst(
					llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0),
					(*block_scope)["this"], "", BB);
		//} else if (NewIdObj * nid = dynamic_cast<NewIdObj * >(e->o)) {
		//} else if (NewTypeObj * ntype = dynamic_cast<NewTypeObj * >(e->o)) {
		} else {
			// load object
			cerr << "Error processing buildExpression: " << endl;
			cerr << "ExpObject unimplemented " << endl;
			return llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 1);

		}

	} else if (ObjectMethodCall* e = dynamic_cast<ObjectMethodCall * >(exp)) {
		//return llvm::CallInst::Create(func_table[*e->o->id+*e->i->id], "", BB);
		// TODO classname
		llvm::Function * F;
		if (NewIdObj * obj = dynamic_cast<NewIdObj *>(e->o)) {
			F = func_table[*obj->i->id + *e->i->id];

		} else if (ThisObj * obj = dynamic_cast<ThisObj *>(e->o)) {
			F = func_table[*_this + *e->i->id];
		//} else if (IdObj * obj = dynamic_cast<NewIdObj * >(e->o)) {
		} else {
			cerr << "Object Type error in ObjectMethodCall";
			abort();
		}

		vector<llvm::Value *> args{llvm::ConstantInt::get(
				llvm::Type::getInt32Ty(Context), 0
		)};
		if (e->e) {
			if (e->e->erlVector) {
				for (auto erl : *e->e->erlVector) {
					args.push_back(buildExpression(erl, Context, BB));
				}

			} else {
				args.push_back(buildExpression(e->e->e, Context, BB));

			}
			return llvm::CallInst::Create(F, std::move(args), "", BB);

		}
		return llvm::CallInst::Create(F, std::move(args), "", BB);

	} else if (ParenExp* e = dynamic_cast<ParenExp * >(exp)) {
		return buildExpression(e->e, Context, BB);

	} else {
		cerr << "Error processing buildExpression: " << endl;
		cerr << "Expresion e cast error " << endl;
		abort();
	}
	
	i->insertInto(BB, BB->end());
	return i;

}

llvm::Instruction *
buildStatement(Statement *s, llvm::LLVMContext &Context, llvm::BasicBlock *BB) {

	if (BlockStatements * block_s = dynamic_cast<BlockStatements * >(s)) {
		llvm::Instruction * i = nullptr;
		// TODO: VarDeclExpList * vdel = nullptr;
		//if (block_s->vdel) {
		//	for (auto var : *block_s->vdel->vdeVector) {
		//	}
		//}
		
		if (block_s->s) {
			for (auto state : *block_s->s->sVector ) {
				i = buildStatement(state, Context, &BB->getParent()->back());
			}
		}

		//if (block_s->vdel) {
		//	for (auto var : *block_s->vdel->vdeVector) {
		//	}
		//}

		return i;

	} else if (IfStatement * if_s = dynamic_cast<IfStatement * >(s)) {
		llvm::Value * cond = buildExpression(if_s->e, Context, BB);
		llvm::BasicBlock * ifBB = 
			llvm::BasicBlock::Create(Context, "if", BB->getParent());

		buildStatement(if_s->s_if, Context, ifBB);
		auto iftmp = &ifBB->getParent()->back();
		
		llvm::BasicBlock * elBB = 
			llvm::BasicBlock::Create(Context, "el", BB->getParent());

		auto el = llvm::BranchInst::Create(ifBB, elBB, cond, BB);
		ifBB = iftmp;
		if (buildStatement(if_s->s_el, Context, elBB) == nullptr) {
			el->removeFromParent();
			delete elBB;
			llvm::BasicBlock * succBB = 
				llvm::BasicBlock::Create(Context, "s", BB->getParent());
			llvm::BranchInst::Create(ifBB, succBB, cond, BB);
			llvm::BranchInst::Create(succBB, ifBB);
			return &succBB->back();
		}

		elBB = &elBB->getParent()->back();

		llvm::BasicBlock * succBB = 
			llvm::BasicBlock::Create(Context, "s", BB->getParent());
		llvm::BranchInst::Create(succBB, elBB);
		llvm::BranchInst::Create(succBB, ifBB);
		return &succBB->back();

	} else if (WhileStatement * while_s = dynamic_cast<WhileStatement * >(s)) {
		// FIXME Cond Block
		// While Block
		// After Block
		//
		llvm::Value * cond = buildExpression(while_s->e, Context, BB);
		llvm::BasicBlock * whileBB = 
			llvm::BasicBlock::Create(Context, "w", BB->getParent());
		
		buildStatement(while_s->s, Context, whileBB);

		llvm::BasicBlock * succBB = 
			llvm::BasicBlock::Create(Context, "s_w", BB->getParent());
		llvm::BranchInst::Create(whileBB, succBB, cond, BB);

		cond = buildExpression(while_s->e, Context, whileBB);
		llvm::BranchInst::Create(whileBB, succBB, cond, whileBB);
		return &succBB->back();

	// TODO: For
	//} else if (ForStatement * for_s = dynamic_cast<ForStatement * >(s)) {
	//	cerr << "ForStatement: unimplemented" << endl;

	} else if (PrintExp * p_exp = dynamic_cast<PrintExp * >(s)) {
		llvm::CallInst *CallPrint = llvm::CallInst::Create(
			_printf, {_exp_format, buildExpression(p_exp->e, Context, BB)},
			"", BB
		);
		return CallPrint;

	} else if (PrintString * p_str = dynamic_cast<PrintString * >(s)) {
		llvm::Constant * StrConstant = 
			llvm::ConstantDataArray::getString(
				Context, p_str->s->str->c_str());
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
		llvm::Constant *StrConstant = llvm::ConstantDataArray::getString(
			Context, *pln_str->s->str + "\n");
		auto *GV = new llvm::GlobalVariable(
			*BB->getParent()->getParent(), StrConstant->getType(),
			true, llvm::GlobalValue::PrivateLinkage,
			StrConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
		GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		GV->setAlignment(llvm::Align(1));
		llvm::CallInst *CallPrint = llvm::CallInst::Create(
			_printf, GV, "", BB
		);
		return CallPrint;
		
	} else if (Assign * assign = dynamic_cast<Assign * >(s)) {
		// TODO assign within a class
		auto store = new llvm::StoreInst(
			buildExpression(assign->e, Context, BB),
			(*block_scope)[*assign->i->id], BB
		);
		store->insertInto(BB, BB->end());
		return store;
		
	// TODO: Array
	//} else if (IndexAssign * idx = dynamic_cast<IndexAssign * >(s)) {
	//	cerr << "IndexAssign: unimplemented" << endl;
		
	} else if (ReturnStatement * ret_s = dynamic_cast<ReturnStatement * >(s)) {
		llvm::ReturnInst::Create(Context, 
			buildExpression(ret_s->e, Context, BB), 
			&BB->getParent()->back());
		
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
	// TODO create constructor
	// map<string, llvm::Value *> class_var_decl;
	if (c->v) {
		func_table[*c->i->id] = llvm::Function::Create(
			llvm::FunctionType::get(
				llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0), false), 
			llvm::Function::ExternalLinkage, *c->i->id, M
		);

		// allocate enough space for the set of variables
		// return value is pointer to variable struct
		//(*block_scope)[*var->i->id] = new llvm::AllocaInst(
		//		llvm::Type::getInt1Ty(Context), 0, "", BB);

		/*for (auto var : *c->v->vdVector) {
			if (dynamic_cast<IntType * >(var->t)) { 
				(*block_scope)[*var->i->id] = new llvm::AllocaInst(
						llvm::Type::getInt32Ty(Context), 0, 
						llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0), 
						llvm::Align(4), "", BB);

			} else if (dynamic_cast<BoolType * >(var->t)) {
				(*block_scope)[*var->i->id] = new llvm::AllocaInst(
						llvm::Type::getInt1Ty(Context), 0, 
						llvm::ConstantInt::getFalse(Context),
						llvm::Align(4), "", BB);
			}
		}*/
	}

	for (auto func : *c->m->mdVector) {
		llvm::FunctionType *FT = nullptr;
		llvm::Type * RT;
		if (dynamic_cast<IntType * >(func->t)) {
			RT = llvm::Type::getInt32Ty(Context);

		} else if (dynamic_cast<BoolType * >(func->t)) {
			RT = llvm::Type::getInt1Ty(Context);

		} else {
			RT = llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0);
		}

		if (func->f) {
			if (func->f->frVector) {
				vector<llvm::Type *> type{llvm::PointerType::get(
						llvm::Type::getInt32Ty(Context), 0)
				};
				for (auto var : *func->f->frVector) {
					if (dynamic_cast<IntType * >(var->t)) {
						type.push_back(llvm::Type::getInt32Ty(Context));

					} else if (dynamic_cast<BoolType * >(var->t)) {
						type.push_back(llvm::Type::getInt1Ty(Context));

					} else {
						type.push_back(llvm::PointerType::get(
									llvm::Type::getInt32Ty(Context), 0));
					}

				}
				FT = llvm::FunctionType::get(RT, std::move(type), false);

			} else {
				if (dynamic_cast<IntType * >(func->f->t)) { 
					FT = llvm::FunctionType::get(
						RT, {llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0), 
						llvm::Type::getInt32Ty(Context) }, false
					);

				} else if (dynamic_cast<BoolType * >(func->f->t)) { 
					FT = llvm::FunctionType::get(
						RT, {llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0),
						llvm::Type::getInt1Ty(Context) }, false
					);

				} else {
					FT = llvm::FunctionType::get(RT,
						{llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0), 
						llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0) }, 
						false
					);
				}
			}

		} else {
			FT = llvm::FunctionType::get(RT, {llvm::PointerType::get(llvm::Type::getInt32Ty(Context), 0)}, false);

		}

		// FIXME: add classname func_table[*c->i->id+*func->i->id] = F;
		func_table[*c->i->id + *func->i->id] = llvm::Function::Create(
			FT, llvm::Function::ExternalLinkage, *c->i->id + *func->i->id, M
		);

	}
	
	for (auto func : *c->m->mdVector) {
		llvm::Function *F = func_table[*c->i->id + *func->i->id];
		_this = c->i->id;
		block_scope = new _BLOCK_SCOPE;
		
		llvm::BasicBlock *BB = 
			llvm::BasicBlock::Create(Context, "EntryBlock", F);

		block_scope->insert("this", new llvm::AllocaInst(
					llvm::Type::getInt32Ty(Context), 0, "", BB), new IdentType(c->i));
		auto store = new llvm::StoreInst(
				F->getArg(0), (*block_scope)["this"], BB);
		store->insertInto(BB, BB->end());

		if (func->f) {
			if (func->f->frVector) {
				unsigned i = 1;
				for (auto var : *func->f->frVector) {
					if (dynamic_cast<IntType * >(var->t)) { 
						block_scope->insert(*var->i->id, new llvm::AllocaInst(
								llvm::Type::getInt32Ty(Context), 0, "", BB), var->t);
						
					} else if (dynamic_cast<BoolType * >(var->t)) {
						block_scope->insert(*var->i->id, new llvm::AllocaInst(
								llvm::Type::getInt1Ty(Context), 0, "", BB), var->t);

					} else { 
						abort();
					}
					auto store = new llvm::StoreInst(
							F->getArg(i++), (*block_scope)[*var->i->id], BB);
					store->insertInto(BB, BB->end());
				}

			} else {
				if (dynamic_cast<IntType * >(func->f->t)) { 
					block_scope->insert(*func->f->i->id, new llvm::AllocaInst(
						llvm::Type::getInt32Ty(Context), 0, "", BB), func->f->t);

				} else if (dynamic_cast<BoolType * >(func->f->t)) {
					block_scope->insert(*func->f->i->id, new llvm::AllocaInst(
						llvm::Type::getInt1Ty(Context), 0, "", BB), func->f->t);

				} else {
					// TODO: ID TYPE
					abort();
				}

				auto store = new llvm::StoreInst(
					F->getArg(1), (*block_scope)[*func->f->i->id], BB
				);
				store->insertInto(BB, BB->end());
			}
		}

		if (func->v) {
			for (auto var : *func->v->vdVector ) {
				if (dynamic_cast<IntType * >(var->t)) { 
					block_scope->insert(*var->i->id, new llvm::AllocaInst(
						llvm::Type::getInt32Ty(Context), 0, 
						llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0), 
						llvm::Align(4), "", BB), var->t);

				} else if (dynamic_cast<BoolType * >(var->t)) {
					block_scope->insert(*var->i->id,  new llvm::AllocaInst(
						llvm::Type::getInt1Ty(Context), 0, 
						llvm::ConstantInt::getFalse(Context),
						llvm::Align(4), "", BB), var->t);
				
				} else {
					cerr << "Error with VarDecl: " << endl;
					cerr << *var->i->id << " is not of an IntType" << endl;
					abort();

				}
			}
		}

		if (func->s) {
			for (auto state : *func->s->sVector ) {
				buildStatement(state, Context, &F->back());
			}
		}

		llvm::ReturnInst::Create(
			Context, buildExpression(func->e, Context, &F->back()), &F->back()
		);

		block_scope->pop_back();
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
	llvm::ReturnInst::Create(
		Context, llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0), &F->back()
	);

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
	//delete M;

}

