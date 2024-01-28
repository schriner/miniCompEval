
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
void Program::evaluate() {
	if (programTypeError) return;
	m->evaluate();
}

// MainClass //
void MainClass::traverse() {
	i1->traverse();
	i2->traverse();
	s->traverse();
	PRINTDEBUGTREE("MainClass");
}
void MainClass::evaluate() {
	s->evaluate();
}

// ClassDeclList //
void ClassDeclList::traverse() {
	for (auto c = cdVector->begin(); c < cdVector->end(); c++) {
		(*c)->traverse();
	}
	PRINTDEBUGTREE("ClassDeclList");
}
//TODO(ss): void ClassDeclList::evaluate() {
//}

// ClassDeclSimple //
void ClassDeclSimple::traverse() {
	i->traverse();
	if (v) { v->traverse(); }
	lowestT->reportBadMethodDecl();
	if (m) { m->traverse(); }
	PRINTDEBUGTREE("ClassDeclSimple");
}
//TODO(ss): void ClassDecl::evaluate() {
//}

// ClassDeclExtends //
void ClassDeclExtends::traverse() {
	i1->traverse();
	i2->traverse();
	if (v) { v->traverse(); }
	lowestT->reportBadMethodDecl();
	if (m) { m->traverse(); }
	PRINTDEBUGTREE("ClassDeclExtends");
}
//TODO(ss): void ClassDecl::evaluate() {
//}

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
//TODO(ss) void VarDecl::evaluate() {
// Account For type
//}

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

// FormalList //
void FormalList::traverse() {
	t->traverse();
	i->traverse();
	if (f) { f->traverse(); }
	PRINTDEBUGTREE("FormalList");
}
void FormalList::setTable(TableNode * ta){
	lowestT = ta;
	t->setTable(ta);
	i->lowestT = ta;
	if (f) { f->setTable(ta); }
}
// FormalRestList //
void FormalRestList::traverse() {
	for (auto f = frVector->begin(); f < frVector->end(); f++) {
		(*f)->traverse();
	}
	PRINTDEBUGTREE("FormalRestList");
}
void FormalRestList::setTable(TableNode * ta){
	lowestT = ta;
	for (auto f = frVector->begin(); f < frVector->end(); f++) {
		(*f)->setTable(ta);
		/*if ((*f)->data) {
			if ( ta->table->find( string(*((*f)->i->id)) ) == ta->table->end() ) {
				(*ta->table)[string(*((*f)->i->id))] = data;
			} else {   
				//ta->appendBadVarDecl(*f);
			}
		}*/
	}
}
// FormalRest //
void FormalRest::traverse() {
	t->traverse();
	i->traverse();
	PRINTDEBUGTREE("FormalRest");
}
void FormalRest::setTable(TableNode * ta){
	lowestT = ta;
	t->setTable(ta);
	i->lowestT = ta;
}
// IntType //
void IntType::traverse() {
	PRINTDEBUGTREE("IntType Leaf");
}
void IntType::setTable(TableNode * t){
	lowestT = t;
}
// BoolType //
void BoolType::traverse() {
	PRINTDEBUGTREE("BoolType Leaf");
}
void BoolType::setTable(TableNode * t){
	lowestT = t;
}
// IdentType //
void IdentType::traverse() {
	i->traverse();
	PRINTDEBUGTREE("IdentType");
}
void IdentType::setTable(TableNode * t){
	lowestT = t;
}
// TypePrime //
void TypePrime::traverse() {
	p->traverse();
	PRINTDEBUGTREE("TypePrime");
}
void TypePrime::setTable(TableNode * ta){
	lowestT = ta;
	p->setTable(ta);
}
// TypeIndexList //
void TypeIndexList::traverse() {
	t->traverse();
	PRINTDEBUGTREE("TypeIndexList");
}
void TypeIndexList::setTable(TableNode * ta){
	lowestT = ta;
	t->setTable(ta);
}

// BlockStatements //
void BlockStatements::traverse() {
	if (s) { s->traverse(); }
	PRINTDEBUGTREE("BlockStatement");
}
void BlockStatements::setTable(TableNode * t) {
	lowestT = t;
	if (s) { s->setTableNodes(t); }
}
void BlockStatements::evaluate() {
	if (s) { s->evaluate(); }
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
void IfStatement::setTable(TableNode * t) {
	lowestT = t;  
	e->setTable(t);
	s_if->setTable(t);
	s_el->setTable(t);
}
void IfStatement::evaluate() {
	if ( e->evaluate() ) {
		s_if->evaluate();
	} else {
		s_el->evaluate();
	}
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
void WhileStatement::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t); 
	s->setTable(t);
}
void WhileStatement::evaluate() {
	while ( e->evaluate() ) {
		s->evaluate();
	}
}

// PrintLineExp //
void PrintLineExp::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("PrintLineExp");
}
void PrintLineExp::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void PrintLineExp::evaluate () {
	cout << e->evaluate() << endl;
}

// PrintLineString //
void PrintLineString::traverse() {
	s->traverse();
	PRINTDEBUGTREE("PrintLineString");
}
void PrintLineString::setTable(TableNode * t) {
	lowestT = t;
	s->lowestT = t;
}
void PrintLineString::evaluate() {
	cout << *(s->str) << endl;
}

// PrintExp //
void PrintExp::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("PrintExp");
}
void PrintExp::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
void PrintExp::evaluate () {
	cout << e->evaluate() << endl;
}

// PrintString //
void PrintString::traverse() {
	s->traverse();
	PRINTDEBUGTREE("PrintString");
}
void PrintString::setTable(TableNode * t) {
	lowestT = t;
	s->lowestT = t;
}
void PrintString::evaluate() {
	cout << *(s->str);
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
void Assign::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
	e->setTable(t);
}
//TODO(ss)void Assign::evaluate() {
//  TypeCheck
//	cout << *(s->str);
//}

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
void IndexAssign::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
	ind->setTable(t);
	e->setTable(t);
}

// ReturnStatement //
void ReturnStatement::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("ReturnStatement");
}
void ReturnStatement::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}
// StatementList //
void StatementList::traverse() {
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		(*s)->traverse();
	}
	PRINTDEBUGTREE("StatementList");
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
void StatementList::evaluate() {
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		(*s)->evaluate();
	}
}

// SingleIndex //
void SingleIndex::traverse() {
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("SingleIndex");
}
void SingleIndex::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}

// MultipleIndices //
void MultipleIndices::traverse() {
	ind->traverse();
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); }
	expErr = false;
	PRINTDEBUGTREE("MultipleIndices");
}
void MultipleIndices::setTable(TableNode * t) {
	lowestT = t;
	ind->setTable(t);
	e->setTable(t);
}

// BinExp //
void BinExp::setTable(TableNode * t) {
	lowestT = t;
	e1->setTable(t); e2->setTable(t);
}

// UnaryExp //
void UnaryExp::setTable(TableNode * t) {
	lowestT = t;
	e->setTable(t);
}

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
int Or::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 || r2;
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
int And::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 && r2;
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
int Equal::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 == r2;
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
int NotEqual::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 != r2;
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
int Lesser::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 < r2;
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
int Greater::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 > r2;
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
int LessEqual::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 <= r2;
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
int GreatEqual::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 >= r2;
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
int Add::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 + r2;
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
int Subtract::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 - r2;
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
int Divide::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 / r2;
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
int Multiply::evaluate() {
	int r1 = e1->evaluate();
	int r2 = e2->evaluate();
	return r1 * r2;
}

// Not //
void Not::traverse() {
	if (expErr) { return; }
	e->traverse();
	if (checkExpResult(b_res)) { expErr = true; }
	PRINTDEBUGTREE("Not");
	updateExpResult(b_res);
}
int Not::evaluate() {
	return !(e->evaluate());
}

// Pos //
void Pos::traverse() {
	if (expErr) { return; }
	e->traverse();
	if (checkExpResult(i_res)) { expErr = true; }
	PRINTDEBUGTREE("Pos");
	updateExpResult(i_res);
}
int Pos::evaluate() {
	// making something positive does absolutely nothing 
	return e->evaluate();
}

// Neg //
void Neg::traverse() {
	if (expErr) { return; }
	e->traverse();
	if (checkExpResult(i_res)) { expErr = true; return; }
	PRINTDEBUGTREE("Neg");
	updateExpResult(i_res);
}
int Neg::evaluate() {
	return -(e->evaluate());
}

// ParenExp //
void ParenExp::traverse() {
	if (expErr) { return; }
	e->traverse();
	PRINTDEBUGTREE("ParenExp");
}
int ParenExp::evaluate() {
	//fprintf(stderr, "ParenExp\n");
	return e->evaluate();
}

// ArrayAccess //
void ArrayAccess::traverse() {
	if (expErr) { return; }
	i->traverse();
	ind->traverse();
	PRINTDEBUGTREE("ArrayAccess");
}
void ArrayAccess::setTable(TableNode * t) {
	if (expErr) { return; }
	lowestT = t;
	i->lowestT = t;
	ind->setTable(t);
}

// Length //
void Length::traverse() {
	if (expErr) { return; }
	i->traverse();
	PRINTDEBUGTREE("Length");
	updateExpResult(i_res);
}
void Length::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
}

// ArrayAccessLength //
void ArrayAccessLength::traverse() {
	if (expErr) { return; }
	i->traverse();
	ind->traverse();
	PRINTDEBUGTREE("ArrayAccessLength");
	updateExpResult(i_res);
}
void ArrayAccessLength::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
	ind->setTable(t);
}

// LitInt //
void LitInt::traverse() {
	if (expErr) { return; }
	i->traverse();
	PRINTDEBUGTREE("LitInt");
	updateExpResult(i_res);
}
void LitInt::setTable(TableNode * t) {
	lowestT = t;
	//fprintf(stderr, "SETTING LitInt");
	i->lowestT = t;
}
int LitInt::evaluate() {
	return i->i;
}

// True //
void True::traverse(){
	if (expErr) { return; }
	PRINTDEBUGTREE("True Leaf");
	updateExpResult(b_res);
}
void True::setTable(TableNode * t) {
	lowestT = t;
}
int True::evaluate() {
	return 1;
}

// False //
void False::traverse(){
	if (expErr) { return; }
	PRINTDEBUGTREE("False Leaf");
	updateExpResult(b_res);
}
void False::setTable(TableNode * t) {
	lowestT = t;
}
int False::evaluate() {
	return 0;
}

// ExpObject //
void ExpObject::traverse(){
	if (expErr) { return; }
	o->traverse();
	PRINTDEBUGTREE("ExpObject");
	//updateExpResult(o_res);
}
void ExpObject::setTable(TableNode * t) {
	lowestT = t;
	o->setTable(t);
}

// ObjectMethodCall //
void ObjectMethodCall::traverse(){
	if (expErr) { return; }
	o->traverse();
	i->traverse();
	if (e) { e->traverse(); }
	PRINTDEBUGTREE("ObjectMethodCall");
}
void ObjectMethodCall::setTable(TableNode * t) {
	lowestT = t;
	o->setTable(t);
	i->lowestT = t;
	if (e) { e->setTable(t); }
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
void IdObj::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
}

// ThisObj //
void ThisObj::traverse() {
	PRINTDEBUGTREE("This LEAF!");
	updateExpResult(o_res);
}
void ThisObj::setTable(TableNode * t) {
	lowestT = t;
}

// NewIdObj //
void NewIdObj::traverse() {
	i->traverse();
	updateExpResult(o_res);
	PRINTDEBUGTREE("NewIdobj");
}
void NewIdObj::setTable(TableNode * t) {
	lowestT = t;
	i->lowestT = t;
}

// NewTypeObj //
void NewTypeObj::traverse() {
	p->traverse();
	i->traverse();
	PRINTDEBUGTREE("NewTypeObj");
	updateExpResult(o_res);
}
void NewTypeObj::setTable(TableNode * t) {
	lowestT = t;
	p->setTable(t);
	i->setTable(t);
}

// ExpRestList //
void ExpRestList::traverse() {
	for (auto e = erVector->begin(); e < erVector->end(); e++) {
		(*e)->traverse();
	}
	PRINTDEBUGTREE("ExpRestList");
}
void ExpRestList::setTable(TableNode * t){
	lowestT = t;
	for (auto e = erVector->begin(); e < erVector->end(); e++) {
		(*e)->setTable(t);
	}
}

// ExpList //
void ExpList::traverse() {
	e->traverse();
	if (erl) { erl->traverse(); }
	PRINTDEBUGTREE("ExpList");
}
void ExpList::setTable(TableNode * t){
	if (erl) { erl->setTable(t); }
	lowestT = t;
	if (e) e->setTable(t);
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


