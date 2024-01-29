
#ifndef ASSEM

/*
 * miniCompEval
 * Sonya Schriner
 * File: TreeNode.cpp
 *
 */

#include "TreeNode.hpp"

using namespace std;

extern Program * programRoot;

void Program::evaluate() {
	  if (programTypeError) return;
		if (c) c->evaluate();
		m->evaluate();
}

void MainClass::evaluate() {
		programRoot->return_reg = 0;
	  s->evaluate();
}

void ClassDeclList::evaluate() {
	if (!cdVector) return;
	for (auto c : *cdVector) {
		c->evaluate();
	}
}
void ClassDeclSimple::evaluate() {
	// add v and method decl to table 
	programRoot->class_table[*i->id] = this;
	for (auto md : *m->mdVector) {
		method_table[*md->i->id] = md;
	}
	// TODO: var_table for an instance of a class
	// cerr << "TODO Var Decl\n";
}

//TODO(ss): void ClassDeclExtends::evaluate() {}

void VarDeclList::evaluate() {
	cerr << "TODO: VarDeclList\n";
}
void VarDecl::evaluate() { 
	cerr << "TODO: VarDeclList\n";
}

int MethodDecl::evaluate() {
	// Used during an actual method call
	programRoot->return_reg = 0;
	map<string, int> variable; // assume no complex types
	programRoot->scope_stack.push_back(&variable);

	if (f && programRoot->arg_stack
			&& programRoot->arg_stack->e && f->t && f->i) {
			variable[*f->i->id] = programRoot->arg_stack->e->evaluate(); 
	}
	if (f && programRoot->arg_stack->erlVector && f->frVector) {
		auto i = 0;
		for (auto param : *f->frVector) {
			variable[*param->i->id] = (*programRoot->arg_stack->erlVector)[i++]->evaluate();
		}
	}
	for (auto var : *v->vdVector) {
		variable[*var->i->id] = 0;
	}
	
	s->evaluate();
	if (programRoot->return_reg) {
		programRoot->arg_stack = nullptr;
		return programRoot->return_reg;
	}

	programRoot->return_reg = e->evaluate();
	programRoot->scope_stack.pop_back();
	return programRoot->return_reg;
}
void MethodDeclList::evaluate() {
	cerr << "TODO: MethodDeclList\n";
}

//TODO(ss): void FormalList::evaluate() {}
//TODO(ss): void FormalRest::evaluate() {}

//TODO(ss): void IntType::evaluate() {}
//TODO(ss): void BoolType::evaluate() {}
//TODO(ss): void IdentType::evaluate() {}
//TODO(ss)(Array): void TypeIndexList::evaluate() {}

void BlockStatements::evaluate() {
	if (s) { s->evaluate(); }
}

void IfStatement::evaluate() {
	if ( e->evaluate() ) {
		s_if->evaluate();
	} else {
		s_el->evaluate();
	}
}

void WhileStatement::evaluate() {
	while ( e->evaluate() ) {
		s->evaluate();
	}
}

void PrintLineExp::evaluate () {
	cout << e->evaluate() << endl;
}

void PrintLineString::evaluate() {
	cout << *(s->str) << endl;
}

void PrintExp::evaluate () {
	cout << e->evaluate() << endl;
}

void PrintString::evaluate() {
	cout << *(s->str);
}

void Assign::evaluate() { 
	(*programRoot->scope_stack.back())[*i->id] = e->evaluate();
  //TODO(ss)
	/*  TypeCheck */ 
	/*  Instance variables */ 
}

//TODO(ss)(Array)void IndexAssign::evaluate() {}

void ReturnStatement::evaluate() {
	cerr <<  "ReturnStatement\n";
	if (!e) { programRoot->return_reg = 100; return; }
	cerr << e->evaluate() << "ReturnStatement\n" << endl;
	programRoot->return_reg = e->evaluate();
}

void StatementList::evaluate() {
	for (auto s = sVector->begin(); s != sVector->end(); s++) {
		(*s)->evaluate();
		if (dynamic_cast<ReturnStatement *>(*s)) { return; }
	}
}

//TODO(ss)(Array)void SingleIndex::evaluate() {}
//TODO(ss)(Array)void MultipleIndices::evaluate() {}

int Or::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 || r2;
}

int And::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 && r2;
}

int Equal::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 == r2;
}

int NotEqual::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 != r2;
}

int Lesser::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 < r2;
}

int Greater::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 > r2;
}

int LessEqual::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 <= r2;
}

int GreatEqual::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 >= r2;
}

int Add::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 + r2;
}

int Subtract::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 - r2;
}

int Divide::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 / r2;
}

int Multiply::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 * r2;
}

int Not::evaluate() {
	return !(e->evaluate());
}

int Pos::evaluate() {
	// making something positive does absolutely nothing
	return e->evaluate();
}

int Neg::evaluate() {
	return -(e->evaluate());
}

int ParenExp::evaluate() {
	//fprintf(stderr, "ParenExp\n");
	return e->evaluate();
}

//TODO(ss)(Array)void ArrayAccess::evaluate() {}
//TODO(ss)(Array)void Length::evaluate() {}
//TODO(ss)(Array)void ArrayAccessLength::evaluate() {}

int LitInt::evaluate() {
	return i->i;
}

int True::evaluate() {
	return 1;
}

int False::evaluate() {
	return 0;
}

int ExpObject::evaluate() {
	if (IdObj * id = dynamic_cast<IdObj *>(o)) {
		//cerr << "ExpObject " << *id->i->id;
		//cerr << (*programRoot->scope_stack.back())[*id->i->id] << endl;
		return (*programRoot->scope_stack.back())[*id->i->id];
	}
	return 11;
}

int ObjectMethodCall::evaluate() {
	// look up the method
	// place the result somewhere
	programRoot->arg_stack = nullptr;
	if (e) programRoot->arg_stack = e;
	if (dynamic_cast<NewIdObj *>(o)) {
		// look up the method in the table and traverse
		// todo arguments 
		// var decl for a class
		ClassDecl * cl = programRoot->class_table[*(dynamic_cast<NewIdObj *>(o)->i)->id];
		//cerr << *i->id << ":" << *(dynamic_cast<NewIdObj *>(o)->i)->id << endl;
		return cl->method_table[*i->id]->evaluate();
		// call the method and evaluate
	} else if (dynamic_cast<IdObj *>(o)){
		cerr << "Err: Trying to make a method call with IdObj\n";
	} else if (dynamic_cast<ThisObj *>(o)){
		cerr << "Err: Trying to make a method call with ThisObj\n";
	} else if (dynamic_cast<NewTypeObj *>(o)){
		cerr << "Err: Trying to make a method call with NewTypeObj\n";
	}
	return 0;
}
//TODO(ss)void IdObj::evaluate() {}
//TODO(ss)void ThisObj::evaluate() {}
void NewIdObj::evaluate() {
	// TODO(ss) create an obj and add it to a table or return the addr
}
//TODO(ss)void NewTypeObj::evaluate() {}

/* list of "exp" or "exp , exp, ..." */
//TODO(ss) void ExpList::evaluate() { // add erl or e to table }

#endif
