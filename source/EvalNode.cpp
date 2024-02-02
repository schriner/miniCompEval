
#ifndef ASSEM

/*
 * miniCompEval
 * Sonya Schriner
 * File: EvalNode.cpp
 *
 */

#include "TreeNode.hpp"

// TODO(ss): Type check during AST generation 

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
	var_table["class"] = {{.name = i->id}, nullptr};
	//cerr << *i->id << endl;
	if (v && v->vdVector) { 
		for (auto var : *v->vdVector) {
			// Type Handler
			if (var) var_table[*var->i->id] = {0, var->t};

		}
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
			//cerr << *f->i->id << " " << variable[*f->i->id].val.exp_array << endl;
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
	if (v && v->vdVector) {
		for (auto var : *v->vdVector) {
			// Type Handler
			Type * type = var->t;

			variable[*var->i->id] = {0, var->t};
		}
	}
	if (s) s->evaluate();
	if (programRoot->return_reg.exp) {
		// Check if it is an early return with value 0;
		programRoot->arg_stack = nullptr;
		programRoot->scope_stack.pop_back();
	  programRoot->call_stack.pop_back();
		return programRoot->return_reg;
	}

	programRoot->arg_stack = nullptr;
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
  //TODO(ss)
	/*  TypeCheck */ 
	if (programRoot->scope_stack.back()->find(*i->id) != programRoot->scope_stack.back()->end()) {
		(*programRoot->scope_stack.back())[*i->id].val = (e->evaluate());
	} else if (programRoot->call_stack.back()->find(*i->id) != programRoot->call_stack.back()->end()) {
		(*programRoot->call_stack.back())[*i->id].val = (e->evaluate());
	} else {
		reportError(" token \"" + *i->id + "\"", "Assign::evaluate() runtime error");
	}
}

void IndexAssign::evaluate() {
	int * array;
	if (programRoot->scope_stack.back()->find(*i->id) != programRoot->scope_stack.back()->end()) {
		array = (*programRoot->scope_stack.back())[*i->id].val.exp_array;
	} else if (programRoot->call_stack.back()->end() != programRoot->call_stack.back()->find(*i->id)) {
		array = (*programRoot->call_stack.back())[*i->id].val.exp_array;
	} else {
		reportError("", "IndexAssign::evaluate() runtime error");
	}
	if (dynamic_cast<SingleIndex *>(ind)) {
		int offset = ind->evaluate();
		array[offset + 2] = e->evaluate().exp;
		return;
	}
	int offset = array[0] + 1;
	int a = 1;
	if (MultipleIndices * m_i = dynamic_cast<MultipleIndices *>(ind)){
		for (auto m : *m_i->ind) {
			offset += array[a++]*(dynamic_cast<SingleIndex *>(m)->e->evaluate().exp); 
		}
	}
	array[offset] = e->evaluate().exp;
/*
	map<string, _SYM>::iterator s;
	if (s = programRoot->scope_stack.back()->find(*i->id);
			s == programRoot->scope_stack.back()->end()) {
		if (s = programRoot->call_stack.back()->find(*i->id);
				s == programRoot->call_stack.back()->end()) {
			reportError("", "IndexAssign::evaluate() runtime error");
		} 
	}

	int offset;
	if (dynamic_cast<SingleIndex *>(ind)) {
		cerr << ind->evaluate() << " " ;
		offset = ind->evaluate() + 2;
	} else if (!dynamic_cast<IdentType * >(s->second.t)) {
		offset = ((int *) s->second.val.exp_array)[0] + 1;
		int a = 1;
		if (MultipleIndices * m_i = dynamic_cast<MultipleIndices *>(ind)){
			for (auto m : *m_i->ind) {
				offset += ((int *) s->second.val.exp_array) [a++]*(dynamic_cast<SingleIndex *>(m)->e->evaluate().exp);
			}
		}
	} else {
		offset = s->second.val.id_array[0].index + 1;
		int a = 1;
		if (MultipleIndices * m_i = dynamic_cast<MultipleIndices *>(ind)){
			for (auto m : *m_i->ind) {
				offset += s->second.val.id_array[a++].index*(dynamic_cast<SingleIndex *>(m)->e->evaluate().exp);
			}
		}
	}

	if (!dynamic_cast<IdentType * >(s->second.t)) {
		cerr << offset << " : " << e->evaluate().exp << endl;
		((int *) s->second.val.exp_array)[offset] = e->evaluate().exp;
	}
	s->second.val.id_array[offset].id = e->evaluate().id;
*/
}

void ReturnStatement::evaluate() {
	cerr <<  "ReturnStatement\n";
	if (!e) { programRoot->return_reg = {100}; return; }
	cerr << e->evaluate().exp << "ReturnStatement\n" << endl;
	programRoot->return_reg = e->evaluate();
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
	
	if (auto s = programRoot->scope_stack.back()->find(*i->id);
			s != programRoot->scope_stack.back()->end()) {
		if (!dynamic_cast<IdentType * >(s->second.t)) {
			array = s->second.val.exp_array;
		} else {
			id_array = s->second.val.id_array;
		}
	} else if (auto s = programRoot->call_stack.back()->find(*i->id);
			s != programRoot->call_stack.back()->end()) {
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
	map<string, _SYM>::iterator s;
	if (s = programRoot->scope_stack.back()->find(*i->id);
			s == programRoot->scope_stack.back()->end()) {
		if (s = programRoot->call_stack.back()->find(*i->id);
				s == programRoot->call_stack.back()->end()) {
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
	map<string, _SYM>::iterator s;
	if (s = programRoot->scope_stack.back()->find(*i->id);
			s == programRoot->scope_stack.back()->end()) {
		if (s = programRoot->call_stack.back()->find(*i->id);
				s == programRoot->call_stack.back()->end()) {
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
		if (programRoot->scope_stack.back()->find(*_id) != programRoot->scope_stack.back()->end()) {
			return (*programRoot->scope_stack.back())[*_id].val;
		} else if (programRoot->call_stack.back()->find(*_id) != programRoot->call_stack.back()->end()) {
			return (*programRoot->call_stack.back())[*_id].val;
		} else {
			reportError(*_id, "ExpObject::evaluate() runtime error with id: ");
		}
	
	} else if (NewIdObj * obj = dynamic_cast<NewIdObj *>(o)) {
		string * _id = obj->i->id;
		ClassDecl * cl = programRoot->class_table[*obj->i->id];
		return {.id = (new map<string, SYM>(cl->var_table))}; // push classname
	
	} else if (dynamic_cast<ThisObj *>(o)) {
		return {.id = programRoot->call_stack.back()};
	
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
	programRoot->arg_stack = nullptr;
	if (e) programRoot->arg_stack = e;
	if (dynamic_cast<NewIdObj *>(o)) {
		// look up the method in the table and traverse
		// var decl for a class
		//programRoot->call_stack.push_back((dynamic_cast<NewIdObj *>(o)->i)->id); // push classname
		ClassDecl * cl = programRoot->class_table[*(dynamic_cast<NewIdObj *>(o)->i)->id];
		programRoot->call_stack.push_back(new map<string, SYM>(cl->var_table) ); // push classname
		//cerr << *i->id << ":" << *(dynamic_cast<NewIdObj *>(o)->i)->id << endl;
		return cl->method_table[*i->id]->evaluate();
 	
	} else if (dynamic_cast<IdObj *>(o)){	
		string * id = (dynamic_cast<IdObj *>(o)->i)->id;
		map<string, SYM> * scope; // current scope with variable
		if (programRoot->scope_stack.back()->find(*id) != programRoot->scope_stack.back()->end()) {
			scope = programRoot->scope_stack.back();
		} else if (programRoot->call_stack.back()->find(*id) != programRoot->scope_stack.back()->end()) {
			scope = programRoot->call_stack.back();
		} else {
			abort();
		}
		ClassDecl * cl = programRoot->class_table[*(*(*scope)[*id].val.id)["class"].val.name];
		programRoot->call_stack.push_back((*scope)[*id].val.id);
		return cl->method_table[*i->id]->evaluate();
		//cerr << "Err: Trying to make a method call with IdObj\n";
	
	} else if (dynamic_cast<ThisObj *>(o)){
		programRoot->call_stack.push_back((programRoot->call_stack.back()));
		ClassDecl * cl = programRoot->class_table[*((*programRoot->call_stack.back())["class"]).val.name];
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
