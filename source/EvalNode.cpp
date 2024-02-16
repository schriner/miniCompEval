
#ifndef ASSEM

/*
 * miniCompEval
 * Sonya Schriner
 * File: EvalNode.cpp
 *
 */

#include "TreeNode.hpp"
#include "Evaluate.hpp"

// TODO(ss): Type check during AST generation 

using namespace std;
using namespace Interpreter;

extern Program * programRoot;
InterpreterContext * context;

void Program::evaluate() {
	context = new InterpreterContext;		
  if (programTypeError) return;
	if (c) c->evaluate();
	m->evaluate();
}

void MainClass::evaluate() {
	context->return_reg = {0};
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
	context->var_table[*i->id]["class"] = {{.name = i->id}, nullptr};
	//cerr << *i->id << endl;
	if (v && v->vdVector) { 
		for (auto var : *v->vdVector) {
			// Type Handler
			if (var) context->var_table[*i->id][*var->i->id] = {0, var->t};

		}
	}
	// TODO: var_table for an instance of a class
	// cerr << "TODO Var Decl\n";
}

void ClassDeclExtends::evaluate() {
	programRoot->class_table[*i->id] = this;
	for (auto md : *m->mdVector) {
		method_table[*md->i->id] = md;
	}
	context->var_table[*i->id]["class"] = {{.name = i->id}, nullptr};
	context->var_table[*i->id]["extends"] = {{.name = i2->id}, nullptr};
	if (v && v->vdVector) { 
		for (auto var : *v->vdVector) {
			// Type Handler
			if (var) {
				// TODO: check for duplicate variable in parent class
				context->var_table[*i->id][*var->i->id] = {0, var->t};
			}
		}
	}
}

void VarDeclList::evaluate() {
	cerr << "TODO: VarDeclList\n";
}
void VarDecl::evaluate() { 
	context->scope_stack.back()->insert(*i->id, {0, t});
}

void VarDeclExpList::evaluate() {
	if (!vdeVector) return;
	for (auto vde = vdeVector->begin(); vde != vdeVector->end(); vde++) {
		(*vde)->evaluate();
	}
}

void VarDeclExp::evaluate() { 
	context->scope_stack.back()->insert(*i->id, {a->e->evaluate(), t});
}

VAL MethodDecl::evaluate() {
	// Used during an actual method call
	context->return_reg = {0};
	SCOPE * scope = new SCOPE();
	MAP * variable = &scope->table; // assume no complex types

	if (f && context->arg_stack
			&& context->arg_stack->e && f->t && f->i) {
			(*variable)[*f->i->id] = {context->arg_stack->e->evaluate(), f->t}; 
			//cerr << *f->i->id << " " << variable[*f->i->id].val.exp_array << endl;
	}
	if (f && context->arg_stack->erlVector && f->frVector) {
		auto i_erl = 0;
		for (auto param : *f->frVector) {
			//cerr << *param->i->id << " : ";
			(*variable)[*param->i->id] = {(*context->arg_stack->erlVector)[i_erl++]->evaluate(), param->t};
			//cerr << variable[*param->i->id] << endl;
		}
	}
	context->scope_stack.push_back(scope);
	if (v && v->vdVector) {
		for (auto var : *v->vdVector) {
			// Type Handler
			Type * type = var->t;

			(*variable)[*var->i->id] = {0, var->t};
		}
	}
	if (s) s->evaluate();
	if (context->return_reg.exp) {
		// Check if it is an early return with value 0;
		context->arg_stack = nullptr;
		delete context->scope_stack.back();
		context->scope_stack.pop_back();
	  context->call_stack.pop_back();
		return context->return_reg;
	}

	context->arg_stack = nullptr;
	context->return_reg = e->evaluate();
	delete context->scope_stack.back();
	context->scope_stack.pop_back();
	context->call_stack.pop_back();
	return context->return_reg;
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
	context->push_nested_scope();
	if (vdel) vdel->evaluate();
	if (s) { s->evaluate(); }
	context->pop_nested_scope();
}

void IfStatement::evaluate() {
	if ( e->evaluate().exp ) {
		s_if->evaluate();
	} else {
		s_el->evaluate();
	}
}

void ForStatement::evaluate() {
	// add variable to scope
	context->push_nested_scope();
	vd->evaluate();
	while ( e->evaluate().exp ) {
		s->evaluate();
		a->evaluate();
	}
	context->pop_nested_scope();
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
  //TODO(ss)
	/* TypeCheck */
	if ( context->scope_stack.size() != 0 && 
			(context->scope_stack.back()->find(*i->id) != context->scope_stack.back()->end())) {
		(*context->scope_stack.back())[*i->id].val = (e->evaluate());
	} else if (context->call_stack.back()->find(*i->id) != context->call_stack.back()->end()) {
		(*context->call_stack.back())[*i->id].val = (e->evaluate());
	} else {
		reportError(" token \"" + *i->id + "\"", "Assign::evaluate() runtime error");
	}
}

void IndexAssign::evaluate() {
	MAP::iterator s;
	if (s = context->scope_stack.back()->find(*i->id);
			s == context->scope_stack.back()->end()) {
		if (s = context->call_stack.back()->find(*i->id);
				s == context->call_stack.back()->end()) {
			reportError("", "IndexAssign::evaluate() runtime error");
		}
	}
	if (dynamic_cast<SingleIndex *>(ind)) {
		int offset = ind->evaluate();
		if (!dynamic_cast<IdentType * >(s->second.t)) {
			s->second.val.exp_array[offset + 2] = e->evaluate().exp;
			return;
		}
		s->second.val.id_array[offset + 2] = {e->evaluate().id};
		return;
	}
	int offset = s->second.val.exp_array[0] + 1;
	int a = 1;
	if (MultipleIndices * m_i = dynamic_cast<MultipleIndices *>(ind)){
		for (auto m : *m_i->ind) {
			offset += s->second.val.exp_array[a++]*(dynamic_cast<SingleIndex *>(m)->e->evaluate().exp); 
		}
	}
	if (!dynamic_cast<IdentType * >(s->second.t)) {
		s->second.val.exp_array[offset] = e->evaluate().exp;
		return;
	}
	s->second.val.id_array[offset] = {e->evaluate().id};

}

void ReturnStatement::evaluate() {
	cerr <<  "ReturnStatement\n";
	if (!e) { context->return_reg = {100}; return; }
	cerr << e->evaluate().exp << "ReturnStatement\n" << endl;
	context->return_reg = e->evaluate();
}

void StatementList::evaluate() {
	if (!sVector) return;
	for (auto s = sVector->begin(); s != sVector->end(); s++) {
		(*s)->evaluate();
		if (dynamic_cast<ReturnStatement *>(*s)) { return; }
	}
}

int SingleIndex::evaluate() {
	return e->evaluate().exp;
}

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
	int * array = nullptr;
	ID_ARRAY * id_array = nullptr;
	
	if (auto s = context->scope_stack.back()->find(*i->id);
			s != context->scope_stack.back()->end()) {
		if (!dynamic_cast<IdentType * >(s->second.t)) {
			array = s->second.val.exp_array;
		} else {
			id_array = s->second.val.id_array;
		}
	} else if (auto s = context->call_stack.back()->find(*i->id);
			s != context->call_stack.back()->end()) {
		if (!dynamic_cast<IdentType * >(s->second.t)) {
			array = s->second.val.exp_array;
		} else {
			id_array = s->second.val.id_array;
		}
	} else {
		reportError("", "Calling ArrayAccess on uninit array");
	}
	
	if (SingleIndex * s_i = dynamic_cast<SingleIndex * >(ind)) {
		if (array) return VAL{array[s_i->evaluate() + 2]};
		return {.id = id_array[s_i->evaluate() + 2].id};
	} else {
		// TODO(ss): Handle index count mismatch
		MultipleIndices * m_i = dynamic_cast<MultipleIndices * >(ind);
		if (array) {
			int offset = array[0] + 1;
			for (int i = 0; i < m_i->ind->size(); i++) {
				// FIXME out of bounds index
				int _i = (dynamic_cast<SingleIndex *>((*m_i->ind)[i]))->e->evaluate().exp;
				offset += array[i+1]*_i;
			}
			return VAL{array[offset]};
		}

		if (!id_array[0].index) return VAL{.id_array = nullptr};
		int offset = id_array[0].index + 1;
		for (int i = 0; i < m_i->ind->size(); i++) {
			// FIXME out of bounds index
			int _i = (dynamic_cast<SingleIndex *>((*m_i->ind)[i]))->e->evaluate().exp;
			offset += id_array[i+1].index*_i;
		}
		return VAL{.id = id_array[offset].id};
	}
	return {0};
}
VAL Length::evaluate() {
	// Check that this id is an array
	MAP::iterator s;
	if (s = context->scope_stack.back()->find(*i->id);
			s == context->scope_stack.back()->end()) {
		if (s = context->call_stack.back()->find(*i->id);
				s == context->call_stack.back()->end()) {
			reportError("", "Calling ArrayAccess on uninit array");
		} 
	}
	if (!dynamic_cast<IdentType * >(s->second.t)) {
		return {((int *) s->second.val.exp_array)[1]};
	} else {
		return {s->second.val.id_array[1].index};
	}
}

VAL ArrayAccessLength::evaluate() {
	// Check that this id is an array
	MAP::iterator s;
	if (s = context->scope_stack.back()->find(*i->id);
			s == context->scope_stack.back()->end()) {
		if (s = context->call_stack.back()->find(*i->id);
				s == context->call_stack.back()->end()) {
			reportError("", "Calling ArrayAccess on uninit array");
		} 
	}

  // Index Count len1, l1n2, len3, ... elemen[0][0][0]
	if (SingleIndex * s_i = dynamic_cast<SingleIndex *>(ind) ) {
		if (!dynamic_cast<IdentType * >(s->second.t)) {
			return {((int *) s->second.val.exp_array)[2]};
		}
		return {s->second.val.id_array[2].index};
	}

	int indx = dynamic_cast<MultipleIndices *>(ind)->ind->size();
	if (!dynamic_cast<IdentType * >(s->second.t)) {
		return {((int *) s->second.val.exp_array)[indx]};
	}
	return {s->second.val.id_array[indx].index};
}

VAL LitInt::evaluate() {
	return {i->i};
}

VAL True::evaluate() {
	return {1};
}

VAL False::evaluate() {
	return {0};
}

// (TYPECHECK:ss)
// Typecheck objects at evaluation time
VAL ExpObject::evaluate() {
	if (IdObj * id = dynamic_cast<IdObj *>(o)) {
		string * _id = id->i->id;
		if (context->scope_stack.back()->find(*_id) != context->scope_stack.back()->end()) {
			return (*context->scope_stack.back())[*_id].val;
		} else if (context->call_stack.back()->find(*_id) != context->call_stack.back()->end()) {
			return (*context->call_stack.back())[*_id].val;
		} else {
			reportError(*_id, "ExpObject::evaluate() runtime error with id: ");
		}
	
	} else if (NewIdObj * obj = dynamic_cast<NewIdObj *>(o)) {
		string * _id = obj->i->id;
		ClassDecl * cl = programRoot->class_table[*obj->i->id];
		return {.id = (new MAP(context->var_table[*_id]))}; // push classname
	
	} else if (dynamic_cast<ThisObj *>(o)) {
		return {.id = context->call_stack.back()};
	
	} else if (NewTypeObj * nto = dynamic_cast<NewTypeObj *>(o)) {
		// (New Array)
		if (SingleIndex * i = dynamic_cast<SingleIndex * >(nto->i)) {
			int array_length = i->e->evaluate().exp; 
			if (!dynamic_cast<IdentType * >(nto->p)) { // Is BOOL or INT
				return {.exp_array = new int[array_length + 2]{1, array_length}};
			} else { 
				return {.id_array = new ID_ARRAY[array_length + 2]{{.index = 1}, {.index = array_length}}};
			}
		// (New MultiArray)
		} else if (MultipleIndices * m_i = dynamic_cast<MultipleIndices * >(nto->i)) {
			int array_length = 1;
			for (auto i : * m_i->ind) {
				array_length *= dynamic_cast<SingleIndex *>(i)->e->evaluate().exp; 
			}
			array_length += 1 + m_i->ind->size();

			if (!dynamic_cast<IdentType * >(nto->p)) { // Is BOOL or INT	
				int * array = new int[array_length]{(int) m_i->ind->size()};
				for (auto i = 0; i < m_i->ind->size(); i++) {
					array[i+1] = dynamic_cast<SingleIndex *>((*m_i->ind)[i])->e->evaluate().exp; 
				}
				return {.exp_array = array};
			} else { 
				ID_ARRAY * array = new ID_ARRAY[array_length]{{.index = (int) m_i->ind->size()}};
				for (auto i = 0; i < m_i->ind->size(); i++) {
					array[i+1].index = dynamic_cast<SingleIndex *>((*m_i->ind)[i])->e->evaluate().exp; 
				}
				return {.id_array = array};
			}
		}
		cerr << "Err: Trying to evaluate and obj with NewTypeObj\n";
	} else {
		cerr << "ExpObject" << endl;
	}
	return {-1};
}

// (TYPECHECK:ss)
// Typecheck objects at evaluation time
VAL ObjectMethodCall::evaluate() {
	// look up the method
	// place the result somewhere
	context->arg_stack = nullptr;
	if (e) context->arg_stack = e;
	if (dynamic_cast<NewIdObj *>(o)) {
		// look up the method in the table and traverse
		// var decl for a class
		ClassDecl * cl = programRoot->class_table[*(dynamic_cast<NewIdObj *>(o)->i)->id];
		context->call_stack.push_back( new MAP(context->var_table[*cl->i->id]) ); // push classname
		for (ClassDecl * parent = cl; dynamic_cast<ClassDeclExtends *>(parent); ) {
			parent = programRoot->class_table[*context->var_table[*parent->i->id]["extends"].val.name];
			context->call_stack.back()->insert(
					context->var_table[*parent->i->id].begin(), context->var_table[*parent->i->id].end()
			);
		}
		return cl->method_table[*i->id]->evaluate();
 	
	} else if (dynamic_cast<IdObj *>(o)){	
		string * id = (dynamic_cast<IdObj *>(o)->i)->id;
		MAP::iterator scope; // current scope with variable
		if (context->scope_stack.back()->find(*id) != context->scope_stack.back()->end()) { 
			scope = context->scope_stack.back()->find(*id);
		} else if (context->call_stack.back()->find(*id) != context->scope_stack.back()->end()) {
			scope = context->call_stack.back()->find(*id);
		} else {
			abort();
		}

		ClassDecl * cl = programRoot->class_table[*(*scope->second.val.id)["class"].val.name];
		context->call_stack.push_back(scope->second.val.id);
		while (dynamic_cast<ClassDeclExtends *>(cl) && cl->method_table.find(*i->id) == cl->method_table.end()) {
			cl = programRoot->class_table[*context->var_table[*cl->i->id]["extends"].val.name];
			context->call_stack.back()->insert(
					context->var_table[*cl->i->id].begin(), context->var_table[*cl->i->id].end()
			);
		}
		return cl->method_table[*i->id]->evaluate();
	
	} else if (dynamic_cast<ThisObj *>(o)) {
		context->call_stack.push_back((context->call_stack.back()));
		ClassDecl * cl = programRoot->class_table[*((*context->call_stack.back())["class"]).val.name];
		while (dynamic_cast<ClassDeclExtends *>(cl) && cl->method_table.find(*i->id) == cl->method_table.end()) {
			cl = programRoot->class_table[*context->var_table[*cl->i->id]["extends"].val.name];
			context->call_stack.back()->insert(
					context->var_table[*cl->i->id].begin(), context->var_table[*cl->i->id].end()
			);
		}
		return cl->method_table[*i->id]->evaluate();

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
