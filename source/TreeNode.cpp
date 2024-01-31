
/*
 * miniCompEval 
 * Sonya Schriner  
 * File: TreeNode.cpp
 *  
 */

#include <vector>
#include <stack>
#include <string>
#include <stdio.h> 
#include <stdlib.h> 
#include <iostream>
#include <fstream>

#include "TreeNode.hpp"

using namespace std;

extern Program * programRoot;

bool expErr = false;
bool isBool = false;
const char * b_res = "BOOL";
const char * i_res = "INT";
const char * o_res = "OBJ";

void updateExpResult (const char * r);
void updateExpResult (string * r);
bool checkExpResult (const char * exp);
bool checkExpResult (string * r);

// Program //
void Program::traverse() {
	m->traverse();
	if (c) { c->traverse(); }
	PRINTDEBUGTREE("Program");
	if (programTypeError) abort();
}

// MainClass //
void MainClass::traverse() {
	i1->traverse();
	i2->traverse();
	s->traverse();
	PRINTDEBUGTREE("MainClass");
}

// ClassDeclList //
void ClassDeclList::traverse() {
	for (auto c = cdVector->begin(); c < cdVector->end(); c++) {
		(*c)->traverse();
	}
	PRINTDEBUGTREE("ClassDeclList");
}

// ClassDeclSimple //
void ClassDeclSimple::traverse() {
	i->traverse();
	if (v) { v->traverse(); }
	lowestT->reportBadMethodDecl();
	if (m) { m->traverse(); }
	PRINTDEBUGTREE("ClassDeclSimple");
}

// ClassDeclExtends //
void ClassDeclExtends::traverse() {
	i->traverse();
	i2->traverse();
	if (v) { v->traverse(); }
	lowestT->reportBadMethodDecl();
	if (m) { m->traverse(); }
	PRINTDEBUGTREE("ClassDeclExtends");
}


// VarDeclList //
void VarDeclList::traverse() {
	/*for (auto v = vdVector->begin(); v < vdVector->end(); v++) {
		//(*v)->traverse();
	} Yes this is a super hacky way of solving this */
	// Report the errors don't traverse anything else
	//if (dynamic_cast<ClassVar*>(lowestT->)){
		//fprintf(stderr, "We have the correct table");
	//}
	lowestT->reportBadVarDecl();
	PRINTDEBUGTREE("VarDeclList");
}
// VarDecl //
void VarDecl::traverse() {
	t->traverse();
	i->traverse();
	PRINTDEBUGTREE("VarDecl");
}


// TableNode - YES I should have a different file for this// 
void TableNode::reportBadVarDecl () {
	for (auto v = redecVarDecl->begin(); v < redecVarDecl->end(); v++) {
		(*v)->reportError();
	}
}
void TableNode::reportBadMethodDecl () {
	for (auto m = redecMethodDecl->begin(); m < redecMethodDecl->end(); m++) {
		(*m)->i->reportError();
	}
}

// MethodDecl //
void MethodDecl::traverse() {
	t->traverse();
	i->traverse();
	if (f) { f->traverse(); }
	if (v) { v->traverse(); }
	if (s) { s->traverse(); }
	e->traverse();
	PRINTDEBUGTREE("MethodDecl");
}

// MethodDeclList //
void MethodDeclList::traverse() {
	for (auto m = mdVector->begin(); m < mdVector->end(); m++) {
		(*m)->traverse(); //fprintf(stderr, "%s\n", (*m)->i->id->c_str());
	}
	PRINTDEBUGTREE("MethodDeclList");
}


// FormalList //
void FormalList::traverse() {
	t->traverse();
	i->traverse();
	if (frVector) { 
		for (auto f = frVector->begin(); f < frVector->end(); f++) {
			(*f)->traverse();
		}
	}
	PRINTDEBUGTREE("FormalList");
}

// FormalRest //
void FormalRest::traverse() {
	t->traverse();
	i->traverse();
	PRINTDEBUGTREE("FormalRest");
}

// IntType //
void IntType::traverse() {
	PRINTDEBUGTREE("IntType Leaf");
}
// BoolType //
void BoolType::traverse() {
	PRINTDEBUGTREE("BoolType Leaf");
}
// IdentType //
void IdentType::traverse() {
	i->traverse();
	PRINTDEBUGTREE("IdentType");
}
// TypeIndexList //
void TypeIndexList::traverse() {
	t->traverse();
	PRINTDEBUGTREE("TypeIndexList");
}

// BlockStatements //
void BlockStatements::traverse() {
	if (s) { s->traverse(); }
	PRINTDEBUGTREE("BlockStatement");
}

// IfStatement //
void IfStatement::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	s_if->traverse();
	s_el->traverse();
	PRINTDEBUGTREE("IfStatement");
}

// WhileStatement //
void WhileStatement::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	s->traverse();
	PRINTDEBUGTREE("WhileStatement");
}
// PrintLineExp //
void PrintLineExp::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("PrintLineExp");
}

// PrintLineString //
void PrintLineString::traverse() {
	s->traverse();
	PRINTDEBUGTREE("PrintLineString");
}

// PrintExp //
void PrintExp::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("PrintExp");
}

// PrintString //
void PrintString::traverse() {
	s->traverse();
	PRINTDEBUGTREE("PrintString");
}

// Assign //
void Assign::traverse() {
	i->traverse();
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); return; }
	TableNode * t = i->lowestT;
	// Type Check 
	while (t->table->find(*(i->id)) == t->table->end() ) {
		// REPORT symbol not found
		if (t->parent == NULL) {
			i->reportError();
			return;
		}
		t = t->parent;
	}
	{
		// Check types
		Data * d = t->table->find(*(i->id))->second;
		if ( checkExpResult(d->type) ) {
	  	i->reportError();
			return;	
		} 
	}
	expErr = false;
	PRINTDEBUGTREE("Assign");
}
// IndexAssign //
void IndexAssign::traverse() {
	i->traverse();
	ind->traverse();
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("IndexAssign");
}

// ReturnStatement //
void ReturnStatement::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("ReturnStatement");
}
// StatementList //
void StatementList::traverse() {
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		(*s)->traverse();
	}
	PRINTDEBUGTREE("StatementList");
}

// SingleIndex //
void SingleIndex::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("SingleIndex");
}

// MultipleIndices //
void MultipleIndices::traverse() {
	/*ind->traverse();
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("MultipleIndices");*/
}

bool Exp::isBoolRes() {
	if (dynamic_cast<BoolResExp *>(this)
			|| dynamic_cast<True *>(this)
			|| dynamic_cast<False *>(this)
			|| dynamic_cast<Not *>(this)
	) {
		return true;
	}
	
	if (dynamic_cast<ExpObject *>(this) 
			|| dynamic_cast<ObjectMethodCall *>(this) 
			|| dynamic_cast<ArrayAccess *>(this)
	) {
		return true;
	}
	if (ParenExp * p = dynamic_cast<ParenExp *>(this)) {
		return p->e->isBoolRes();
	}
	return false;
}

bool Exp::isIntRes() {
	// (TYPECHECK:ss)
	// if not integer literal or intres expr or id with type int type error
	// if ObjectMethodCall check e type
	// if ExpObject fixme
	// check they type of the expr within the parentheses
	if (dynamic_cast<IntResExp *>(this)
			|| dynamic_cast<LitInt *>(this)
			|| dynamic_cast<Pos *>(this)
			|| dynamic_cast<Neg *>(this)
			|| dynamic_cast<Length *>(this)
			|| dynamic_cast<ArrayAccessLength *>(this)
	) {
		return true;
	}
	//if (ObjectMethodCall * o = dynamic_cast<ObjectMethodCall *>(this)) {
	//if (ExpObject * o = dynamic_cast<ExpObject *>(this)) {
	if (dynamic_cast<ExpObject *>(this) 
			|| dynamic_cast<ObjectMethodCall *>(this)
			|| dynamic_cast<ArrayAccess *>(this)
	) {
		// check the return type in the obj after a table lookup
		//return o->e->isIntRes();
		// check the return type in the obj after a table lookup
		//return o->e->isIntRes();
		return true;
	}
	if (ParenExp * p = dynamic_cast<ParenExp *>(this)) {
		return p->e->isIntRes();
	}
	return false;
}
// BinExp //

// UnaryExp //

// Or //
void Or::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(b_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(b_res)) { expErr = true; }
	PRINTDEBUGTREE("Or");
	updateExpResult(b_res);
}

// And //
void And::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(b_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(b_res)) { expErr = true; }
	PRINTDEBUGTREE("And");
	updateExpResult(b_res);
}

// Equal //
void Equal::traverse(){
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(b_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(b_res)) { expErr = true; }
	PRINTDEBUGTREE("Equal");
	updateExpResult(b_res);
}

// NotEqual //
void NotEqual::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(b_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(b_res)) { expErr = true; }
	PRINTDEBUGTREE("NotEqual");
	updateExpResult(b_res);
}

// Lesser //
void Lesser::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Lesser");
	updateExpResult(b_res);
}

// Greater //
void Greater::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Greater");
	updateExpResult(b_res);
}

// LessEqual //
void LessEqual::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("LessEqual");
	updateExpResult(b_res);
}

// GreatEqual //
void GreatEqual::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("GreatEqual");	
	updateExpResult(b_res);
}

// Add //
void Add::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Add");
	updateExpResult(i_res);
}

// Subtract //
void Subtract::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Subtract");
	updateExpResult(i_res);
}

// Divide //
void Divide::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Divide");
	updateExpResult(i_res);
}

// Multiply //
void Multiply::traverse() {
	if (expErr) { return; }
	e1->traverse(); 	
	if (checkExpResult(i_res)) { expErr = true; }
	if (expErr) { return; }
	e2->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Multiply");
	updateExpResult(i_res);
}

// Not //
void Not::traverse() {
	if (expErr) { return; }
	e->traverse();
	if (checkExpResult(b_res)) { expErr = true; }
	PRINTDEBUGTREE("Not");
	updateExpResult(b_res);
}

// Pos //
void Pos::traverse() {
	if (expErr) { return; }
	e->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Pos");
	updateExpResult(i_res);
}

// Neg //
void Neg::traverse() {
	if (expErr) { return; }
	e->traverse();
	if (checkExpResult(i_res)) { expErr = true; return; }
	PRINTDEBUGTREE("Neg");
	updateExpResult(i_res);
}

// ParenExp //
void ParenExp::traverse() {
	if (expErr) { return; }
	e->traverse();
	PRINTDEBUGTREE("ParenExp");
}

// ArrayAccess //
void ArrayAccess::traverse() {
	if (expErr) { return; }
	i->traverse();
	ind->traverse();
	PRINTDEBUGTREE("ArrayAccess");
}

// Length //
void Length::traverse() {
	if (expErr) { return; }
	i->traverse();
	PRINTDEBUGTREE("Length");
	updateExpResult(i_res);
}

// ArrayAccessLength //
void ArrayAccessLength::traverse() {
	if (expErr) { return; }
	i->traverse();
	ind->traverse();
	PRINTDEBUGTREE("ArrayAccessLength");
	updateExpResult(i_res);
}

// LitInt //
void LitInt::traverse() {
	if (expErr) { return; }
	i->traverse();
	PRINTDEBUGTREE("LitInt");
	updateExpResult(i_res);
}

// True //
void True::traverse(){
	if (expErr) { return; }
	PRINTDEBUGTREE("True Leaf");
	updateExpResult(b_res);
}

// False //
void False::traverse(){
	if (expErr) { return; }
	PRINTDEBUGTREE("False Leaf");
	updateExpResult(b_res);
}

// ExpObject //
void ExpObject::traverse(){
	if (expErr) { return; }
	o->traverse();
	PRINTDEBUGTREE("ExpObject");
	//updateExpResult(o_res);
}

// ObjectMethodCall //
void ObjectMethodCall::traverse(){
	if (expErr) { return; }
	o->traverse();
	i->traverse();
	if (e) { e->traverse(); }
	PRINTDEBUGTREE("ObjectMethodCall");
}

// IdObj //
void IdObj::traverse() {
	i->traverse();
	PRINTDEBUGTREE("IdObj");
	TableNode * t = lowestT;
	// Now we gonna check that type
	while (t->table->find(*(i->id)) == t->table->end() ) {
		// ok symbol not there REPORT
		if (t->parent == NULL) {
			expErr = true;
			//i->reportError();
			return;
		}
		t = t->parent;
	}
	Data * d = t->table->find(*(i->id))->second;
	updateExpResult( d->type );
}

// ThisObj //
void ThisObj::traverse() {
	PRINTDEBUGTREE("This LEAF!");
	updateExpResult(o_res);
}

// NewIdObj //
void NewIdObj::traverse() {
	i->traverse();
	updateExpResult(o_res);
	PRINTDEBUGTREE("NewIdobj");
}

// NewTypeObj //
void NewTypeObj::traverse() {
	p->traverse();
	i->traverse();
	PRINTDEBUGTREE("NewTypeObj");
	updateExpResult(o_res);
}

// ExpList //
void ExpList::traverse() {
	if (e) e->traverse();
	if (erlVector) { 
		for (auto e = erlVector->begin(); e < erlVector->end(); e++) {
			(*e)->traverse();
		}
	}
	PRINTDEBUGTREE("ExpList");
}


// Ident //
void Ident::traverse() {
	//fprintf(stderr, "At ID Leaf: %s\n", id->c_str());
}

// StringLiteral //
void StringLiteral::traverse() {
	//fprintf(stderr, "At StringLiteral Leaf: %s\n", str->c_str());
}

// IntLiteral //
void IntLiteral::traverse() {
	//fprintf(stderr, "At Leaf: %d\n", i);
}

#ifdef ASSEM
void VarDecl::setTable(TableNode * ta){
	lowestT = ta;
	t->setTable(ta);
	i->lowestT = ta;
	if (data) { 
		if ( ta->table->find( string(*(i->id)) ) == ta->table->end() ) {
			(*ta->table)[ string(*(i->id)) ] = data; 
		} else {
			lowestT->appendBadVarDecl(this);
		}
	}
}

void MethodDeclList::setTableParentNodes(TableNode * p) {
	for (auto m = mdVector->begin(); m < mdVector->end(); m++) {
		(*m)->lowestT->parent = p; 
		if ((*m)->data) {
			if ( p->table->find( string(*((*m)->i->id)) ) == p->table->end() ) {
				(*p->table)[string(*((*m)->i->id))] = data;
			} else {   
				p->appendBadMethodDecl(*m);
			}
		}
	}
}

void FormalList::setTable(TableNode * ta){
	lowestT = ta;
	if(t && i) {
		t->setTable(ta);
		i->lowestT = ta;
	}
	if (frVector) {
		for (auto f = frVector->begin(); f < frVector->end(); f++) {
			(*f)->setTable(ta);
		}
	}
}
void FormalRest::setTable(TableNode * ta){
	lowestT = ta;
	t->setTable(ta);
	i->lowestT = ta;
}
void IntType::setTable(TableNode * t){
	lowestT = t;
}
void BoolType::setTable(TableNode * t){
	lowestT = t;
}
void IdentType::setTable(TableNode * t){
	lowestT = t;
}
void TypeIndexList::setTable(TableNode * ta){
	lowestT = ta;
	t->setTable(ta);
}
void BlockStatements::setTable(TableNode * t) {
	lowestT = t;
	if (s) { s->setTableNodes(t); }
}
void IfStatement::setTable(TableNode * t) {
	lowestT = t;  
	e->setTable(t);
	s_if->setTable(t);
	s_el->setTable(t);
}
void WhileStatement::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t); 
	s->setTable(t);
}

void PrintLineExp::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void PrintLineString::setTable(TableNode * t) {
	lowestT = t;
	s->lowestT = t;
}
void PrintExp::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void PrintString::setTable(TableNode * t) {
	lowestT = t;
	s->lowestT = t;
}
void Assign::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
	e->setTable(t);
}
void IndexAssign::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
	ind->setTable(t);
	e->setTable(t);
}
void ReturnStatement::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void StatementList::setTableNodes(TableNode * n) {
	// - add code to set the contents of each statement lowestT
	// - make virtual method for this in each statement
	// - make function for each statement
	lowestT = n;
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		//(*s)->lowestT = n;
		(*s)->setTable(n);
	}
}
void SingleIndex::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void MultipleIndices::setTable(TableNode * t) {
	lowestT = t;
	//ind->setTable(t);
	//e->setTable(t);
}
void BinExp::setTable(TableNode * t) {
	lowestT = t;
	e1->setTable(t); e2->setTable(t);
}
void UnaryExp::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void ArrayAccess::setTable(TableNode * t) {
	if (expErr) { return; }
	lowestT = t;
	i->lowestT = t;
	ind->setTable(t);
}
void Length::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
}
void ArrayAccessLength::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
	ind->setTable(t);
}
void LitInt::setTable(TableNode * t) {
	lowestT = t;
	//fprintf(stderr, "SETTING LitInt");
	i->lowestT = t;
}
void True::setTable(TableNode * t) {
	lowestT = t;
}
void False::setTable(TableNode * t) {
	lowestT = t;
}
void ExpObject::setTable(TableNode * t) {
	lowestT = t;
	o->setTable(t);
}
void ObjectMethodCall::setTable(TableNode * t) {
	lowestT = t;
	o->setTable(t);
	i->lowestT = t;
	if (e) { e->setTable(t); }
}
void IdObj::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
}
void ThisObj::setTable(TableNode * t) {
	lowestT = t;
}
void NewIdObj::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
}
void NewTypeObj::setTable(TableNode * t) {
	lowestT = t;
	p->setTable(t);
	i->setTable(t);
}
void ExpList::setTable(TableNode * t){
	if (erlVector) { 
		for (auto e = erlVector->begin(); e < erlVector->end(); e++) {
			(*e)->setTable(t);
		}
	}
	lowestT = t;
	if (e) e->setTable(t);
}
#endif
