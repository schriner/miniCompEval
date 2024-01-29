
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
		programRoot->return_reg = {0};
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

VAL MethodDecl::evaluate() {
	// Used during an actual method call
	programRoot->return_reg = {0};
	map<string, SYM> variable; // assume no complex types

	if (f && programRoot->arg_stack
			&& programRoot->arg_stack->e && f->t && f->i) {
			variable[*f->i->id] = {programRoot->arg_stack->e->evaluate(), f->t}; 
	}
	if (f && programRoot->arg_stack->erlVector && f->frVector) {
		auto i_erl = 0;
		for (auto param : *f->frVector) {
			//cerr << *param->i->id << " : ";
			variable[*param->i->id] = {(*programRoot->arg_stack->erlVector)[i_erl++]->evaluate(), param->t};
			//cerr << variable[*param->i->id] << endl;
		}
	}
	programRoot->scope_stack.push_back(&variable);
	for (auto var : *v->vdVector) {
		// Type Handler
		Type * type = var->t;

		variable[*var->i->id] = {0, var->t};
	}
	
	s->evaluate();
	if (programRoot->return_reg.exp) {
		// Check if it is an early return with value 0;
		programRoot->arg_stack = nullptr;
		programRoot->scope_stack.pop_back();
		return programRoot->return_reg;
	}

	programRoot->return_reg = e->evaluate();
	programRoot->scope_stack.pop_back();
	programRoot->call_stack.pop_back();
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

//TODO(ss)(Array):void TypeIndexList::evaluate() {}

void BlockStatements::evaluate() {
	if (s) { s->evaluate(); }
}

void IfStatement::evaluate() {
	if ( e->evaluate().exp ) {
		s_if->evaluate();
	} else {
		s_el->evaluate();
	}
}

void WhileStatement::evaluate() {
	while ( e->evaluate().exp ) {
		s->evaluate();
	}
}

void PrintLineExp::evaluate () {
	cout << e->evaluate().exp << endl;
}

void PrintLineString::evaluate() {
	cout << *(s->str) << endl;
}

void PrintExp::evaluate () {
	cout << e->evaluate().exp;
}

void PrintString::evaluate() {
	cout << *(s->str);
}

void Assign::evaluate() { 
	(*programRoot->scope_stack.back())[*i->id].val = (e->evaluate());
  //TODO(ss)
	/*  TypeCheck */ 
	/*  Instance variables */ 
}

//TODO(ss)(ArrayMulti)
void IndexAssign::evaluate() {
	int offset = ind->evaluate();
	(*programRoot->scope_stack.back())[*i->id].val.exp_single[offset + 1] = e->evaluate().exp;
}

void ReturnStatement::evaluate() {
	cerr <<  "ReturnStatement\n";
	if (!e) { programRoot->return_reg = {100}; return; }
	cerr << e->evaluate().exp << "ReturnStatement\n" << endl;
	programRoot->return_reg = e->evaluate();
}

void StatementList::evaluate() {
	for (auto s = sVector->begin(); s != sVector->end(); s++) {
		(*s)->evaluate();
		if (dynamic_cast<ReturnStatement *>(*s)) { return; }
	}
}

int SingleIndex::evaluate() {
	return e->evaluate().exp;
}
//TODO(ss)(ArrayMulti)void MultipleIndices::evaluate() {}

VAL Or::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp || r2.exp};
}

VAL And::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp && r2.exp};
}

VAL Equal::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp == r2.exp};
}

VAL NotEqual::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp != r2.exp};
}

VAL Lesser::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp < r2.exp};
}

VAL Greater::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp > r2.exp};
}

VAL LessEqual::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp <= r2.exp};
}

VAL GreatEqual::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp >= r2.exp};
}

VAL Add::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp + r2.exp};
}

VAL Subtract::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp - r2.exp};
}

VAL Divide::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp / r2.exp};
}

VAL Multiply::evaluate() {
	VAL r1 = e1->evaluate();
	VAL r2 = e2->evaluate();
	return {r1.exp * r2.exp};
}

VAL Not::evaluate() {
	return {!(e->evaluate().exp)};
}

VAL Pos::evaluate() {
	// making something positive does absolutely nothing
	return e->evaluate();
}

VAL Neg::evaluate() {
	return {-(e->evaluate().exp)};
}

VAL ParenExp::evaluate() {
	//fprintf(stderr, "ParenExp\n");
	return e->evaluate();
}

VAL ArrayAccess::evaluate() {
	if (SingleIndex * s_i = dynamic_cast<SingleIndex * >(ind)) {
		if ((*programRoot->scope_stack.back())[*i->id].val.exp_single == nullptr) {
			cerr << "Calling Length on uninit array";
			return {0};
		}
		return VAL{(*programRoot->scope_stack.back())[*i->id].val.exp_single[s_i->e->evaluate().exp + 1]};
	}
	return {0};
}
VAL Length::evaluate() {
	// Check that this id is an array
	if ((*programRoot->scope_stack.back())[*i->id].val.exp_single == nullptr) {
		cerr << "Calling Length on uninit array";
		return {0};
	}
	return {(*programRoot->scope_stack.back())[*i->id].val.exp_single[0]};
}

//TODO(ss)(ArrayMulti)void ArrayAccessLength::evaluate() {}

VAL LitInt::evaluate() {
	return {i->i};
}

VAL True::evaluate() {
	return {1};
}

VAL False::evaluate() {
	return {0};
}

VAL ExpObject::evaluate() {
	if (IdObj * id = dynamic_cast<IdObj *>(o)) {
		//cerr << "ExpObject " << *id->i->id;
		//cerr << (*programRoot->scope_stack.back())[*id->i->id] << endl;
		return (*programRoot->scope_stack.back())[*id->i->id].val;
	} else if (dynamic_cast<NewIdObj *>(o)) {
		cerr << "Err: Trying to evaluate and obj with NewIdObj\n";
	} else if (dynamic_cast<ThisObj *>(o)) {
		cerr << "Err: Trying to evaluate and obj with ThisObj\n";
	} else if (NewTypeObj * nto = dynamic_cast<NewTypeObj *>(o)) {
		// NEW prime_type index 
		// (Array)
		if (SingleIndex * i = dynamic_cast<SingleIndex * >(nto->i)) {
			if (!dynamic_cast<IdentType * >(nto->p)) { // Is BOOL or INT
				int array_length = i->e->evaluate().exp; 
				int * array = new int[array_length + 1];
				array[0] = array_length;
				return {.exp_single = array};
			} else { /* TODO(IdentType) */}
		}
		
		// TODO: (MultiArray)
		cerr << "Err: Trying to evaluate and obj with NewTypeObj\n";
	} else {
		cerr << "ExpObject" << endl;
	}
	return {11};
}

VAL ObjectMethodCall::evaluate() {
	// look up the method
	// place the result somewhere
	programRoot->arg_stack = nullptr;
	if (e) programRoot->arg_stack = e;
	if (dynamic_cast<NewIdObj *>(o)) {
		// look up the method in the table and traverse
		// var decl for a class
		programRoot->call_stack.push_back((dynamic_cast<NewIdObj *>(o)->i)->id); // push classname
		ClassDecl * cl = programRoot->class_table[*(dynamic_cast<NewIdObj *>(o)->i)->id];
		//cerr << *i->id << ":" << *(dynamic_cast<NewIdObj *>(o)->i)->id << endl;
		return cl->method_table[*i->id]->evaluate();
	
	} else if (dynamic_cast<IdObj *>(o)){
		cerr << "Err: Trying to make a method call with IdObj\n";
	
	} else if (dynamic_cast<ThisObj *>(o)){
		programRoot->call_stack.push_back((programRoot->call_stack.back()));
		ClassDecl * cl = programRoot->class_table[*(programRoot->call_stack.back())];
		//cerr << "This: " << *(programRoot->call_stack.back()) << " " << *i->id << endl;
		return cl->method_table[*i->id]->evaluate();

		//cerr << "Err: Trying to make a method call with ThisObj\n";
		//ClassDecl * cl = programRoot->class_table[*(dynamic_cast<NewIdObj *>(o)->i)->id];
	} else if (dynamic_cast<NewTypeObj *>(o)){
		cerr << "Err: Trying to make a method call with NewTypeObj\n";
	} else {
		cerr << "Methodcall" << endl;
	}

	return {0};
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
