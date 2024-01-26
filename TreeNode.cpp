
/*
 * CS352: Project 3 
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

extern fstream assemStream;
extern Program * programRoot;

string * ExpResult = NULL;
bool expErr = false;
bool isBool = false;
const char * b_res = "BOOL";
const char * i_res = "INT";
const char * o_res = "OBJ";
int ifCnt = 0;
int lCnt = 0;
int strCnt = 0;
int expCnt = 0;
int regCnt = 0;
vector< map<string, string*> * > nameTableStack;
vector< map<string, string*> * > typeTableStack;

void updateExpResult (const char * r) {
	if (ExpResult) { delete ExpResult; }
	ExpResult = new string(r);
}
void updateExpResult (string * r) {
	if (ExpResult) { delete ExpResult; }
	ExpResult = new string(*r);
}
//string * branchPrint(string * str, string * stmt_str){
void branchPrint(string * str, string * stmt_str){
	string s("ldr r0, =");
	s = s + str->c_str() + "\n\tbl printf\n";
	*stmt_str = *stmt_str + s;
	//return new string(s);
}

bool checkExpResult (const char * exp) {
	return ExpResult->compare(exp);
}
bool checkExpResult (string * r) {
	return ExpResult->compare(*r);
}

bool isIntLiteral (Exp * e) {
	if (dynamic_cast<LitInt *>( e )) { // op1 is a literal
		return true;
	} 
	if (dynamic_cast<ParenExp *>( e )) { // check contents of parentheses
		ParenExp * expr = (ParenExp * ) e;
		return isIntLiteral(expr->e);
	}
	if (dynamic_cast<Neg *> ( e )) { // check the negative
		Neg * expr = (Neg * ) e;
		return isIntLiteral(expr->e);
	}
	if (dynamic_cast<Pos *> ( e )) {
		Pos * expr = (Pos * ) e;
		return isIntLiteral(expr->e);
	}
	return false; // expr is false
}
void getIntLiteral (Exp * e, string * s) {
	if (dynamic_cast<LitInt *>( e )) { // op1 is a literal
		LitInt * expr = (LitInt *) e;
		*s = *s + "#" + to_string(expr->i->i);
		return;
	} 

	int negCnt = 0;
	while (dynamic_cast<ParenExp *>( e ) || dynamic_cast<Neg *> ( e ) 
					|| dynamic_cast<LitInt *>( e ) || dynamic_cast<Pos *> ( e )) {
		if (dynamic_cast<ParenExp *>( e )) { // ignore parentheses
			ParenExp * expr = (ParenExp * ) e;
			e = expr->e;
		} else if (dynamic_cast< Pos * >( e )) {
			Pos * expr = (Pos * ) e;
			e = expr->e;
		} else if (dynamic_cast< Neg * > ( e )) { // count the negative
			negCnt++;
			Neg * expr = (Neg * ) e;
			e = expr->e;
		} else if (dynamic_cast<LitInt *>( e )) { // stop if u hit the lit
			LitInt * expr = (LitInt *) e;
			if (negCnt % 2) { 
				*s = *s + "#-" + to_string(expr->i->i);
			} else {
				*s = *s + "#" + to_string(expr->i->i);
			}
			return;
		}
	}
	cout << "You goofed :/" << endl; 

}

// Program //
void Program::traverse() {
	m->traverse();
	if (c) { c->traverse(); }
	PRINTDEBUGTREE("Program");
}
void Program::evaluate() {
	m->evaluate();
}
void Program::assem() {
	m->assem();
	if (c) { c->assem(); }

	// .data: global variables here
	assemStream << ".data"<< endl;
	for (auto d = dataSection->begin(); d < dataSection->end(); d++) {
		assemStream << "\t" << **d << endl;
	}
	assemStream << endl;
	
	// .text: write string constants here
	assemStream << ".text"<< endl;
	for (auto t = textSection->begin(); t < textSection->end(); t++) {
		assemStream << "\t" << **t << endl;
	}
	assemStream << endl;

	// .main
	assemStream << ".global main\n.balign 4\nmain:\n";
	assemStream << "\tpush {lr}\n" << endl;
	//assemStream << "\tldr r9, =" + string(PRINT_EXP) << endl;
	for (auto i = m->instr->begin(); i < m->instr->end(); i++) {
		if ((**i)[0] != '\t') { assemStream << "\t"; }
		assemStream << **i << endl;
	}
	assemStream << "\tpop {pc}" << endl;
	
	// .class
	if (c) { 
		//assemStream << "." << << endl;
		for (auto i = c->instr->begin(); i < c->instr->end(); i++) {
			assemStream << **i ;
		}
	//	assemStream << endl;
	}
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
void MainClass::assem() {
	string str("");
	s->assem(&str, NULL);
	instr->push_back( new string(str) );
}

// ClassDeclList //
void ClassDeclList::traverse() {
	for (auto c = cdVector->begin(); c < cdVector->end(); c++) {
		(*c)->traverse();
	}
	PRINTDEBUGTREE("ClassDeclList");
}
void ClassDeclList::assem() {
	for (auto c = cdVector->begin(); c < cdVector->end(); c++) {
		(*c)->assem();
	}
}

// ClassDeclSimple //
void ClassDeclSimple::traverse() {
	i->traverse();
	if (v) { v->traverse(); }
	lowestT->reportBadMethodDecl();
	if (m) { m->traverse(); }
	PRINTDEBUGTREE("ClassDeclSimple");
}
void ClassDeclSimple::assem() {
	programRoot->c->instr->push_back(new string (*(i->id) + ":\n"));
	nameTableStack.push_back(nameTable);
	typeTableStack.push_back(typeTable);
	if (v) { v->assem(nameTable, typeTable); }
	if (m) { m->assem(i->id); }
	typeTableStack.pop_back();
	nameTableStack.pop_back();
}

// ClassDeclExtends //
void ClassDeclExtends::traverse() {
	i1->traverse();
	i2->traverse();
	if (v) { v->traverse(); }
	lowestT->reportBadMethodDecl();
	if (m) { m->traverse(); }
	PRINTDEBUGTREE("ClassDeclExtends");
}
void ClassDeclExtends::assem() {
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
void VarDeclList::assem( map<string, string *> * nameTable, map<string, string*> * typeTable) {
	for (auto v = vdVector->begin(); v < vdVector->end(); v++) {
		(*v)->assem(nameTable, typeTable);
	}
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
		//fprintf(stderr, "%s\n", i->id->c_str());
		if ( ta->table->find( string(*(i->id)) ) == ta->table->end() ) {
			(*ta->table)[ string(*(i->id)) ] = data; 
		} else {
			lowestT->appendBadVarDecl(this);
		}
	}
}
void VarDecl::assem( map<string, string * > * nameTable, map<string, string*> * typeTable) {
	if (dynamic_cast< TypePrime* >( t )){
		TypePrime * p = (TypePrime * ) t;
		if (dynamic_cast< BoolType* >( p->p ) 
				|| dynamic_cast< IntType* >( p->p )) {
			//cout << "I'm here" << endl;
			programRoot->dataSection->push_back(new string("sReg" + to_string(regCnt) + ": .word 0"));
			label = new string("sReg" + to_string(regCnt++));
			(*nameTable)[*(i->id)] = label;
			if (dynamic_cast< BoolType* >( p->p )) {
				(*typeTable)[*(i->id)] = new std::string("BOOL");
			} else {
				(*typeTable)[*(i->id)] = new std::string("INT");
			}
		}
	} else { // lets assume it's 1 d
		TypeIndexList * p = (TypeIndexList * ) t;
		if (dynamic_cast< BoolType* >( p->p ) 
				|| dynamic_cast< IntType* >( p->p )) {
			//cout << "I'm here" << endl;
			programRoot->dataSection->push_back(new string("sReg" + to_string(regCnt) + ": .word 0"));
			label = new string("sReg" + to_string(regCnt++));
			(*nameTable)[*(i->id)] = label;
			if (dynamic_cast< BoolType* >( p->p )) {
				(*typeTable)[*(i->id)] = new std::string("BOOL_A");
			} else {
				(*typeTable)[*(i->id)] = new std::string("INT_A");
			}
		}
	}
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
void MethodDecl::assem(string *objName) {
	// TODO: Make sure this is handled properly
	if (v) {v->assem(nameTable, typeTable);}

	nameTableStack.push_back(nameTable);
	typeTableStack.push_back(typeTable);
	
	programRoot->c->instr->push_back(new string (*objName + *(i->id) + ":\n\tpush {lr}\n\n"));
	string stmt_str("");
	s->assem(&stmt_str, nameTable);
	programRoot->c->instr->push_back(new string ("\t" + stmt_str + "\n"));
	stmt_str = string("");
	if (s) { e->assem(&stmt_str, NULL); }
	programRoot->c->instr->push_back(new string (stmt_str + "\n\tmov r0, r1\n"));
	programRoot->c->instr->push_back(new string ("\n\tpop {pc}\n"));

	typeTableStack.pop_back();
	nameTableStack.pop_back();
}

// MethodDeclList //
void MethodDeclList::traverse() {
	for (auto m = mdVector->begin(); m < mdVector->end(); m++) {
		(*m)->traverse(); //fprintf(stderr, "%s\n", (*m)->i->id->c_str());
	}
	PRINTDEBUGTREE("MethodDeclList");
}
void MethodDeclList::setTableParentNodes(TableNode * p){
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
void MethodDeclList::assem(string * objName) {
	for (auto m = mdVector->begin(); m < mdVector->end(); m++) {
		(*m)->assem(objName); 
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
void BlockStatements::assem(string * stmt_str, map<string, string*> * nameTable)	{
	//return s->assem();
	s->assem(stmt_str, nameTable);
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
void IfStatement::assem(string * stmt_str, map<string, string*> * nameTable)	{
	//string s("");
	string label("if" + to_string(ifCnt));
	e->assem(stmt_str, &label);

  *stmt_str = *stmt_str + "\t";
	s_if->assem(stmt_str, nameTable);
	
	*stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + "\tb el" + to_string(ifCnt);
	*stmt_str = *stmt_str + "\n" + label + ":\n\t";
	
	s_el->assem(stmt_str, nameTable);
	label = string("el" + to_string(ifCnt));
	*stmt_str = *stmt_str + label + ":\n";
	
	ifCnt++;
	//return new string("IF STATEMENT");
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
void WhileStatement::assem(string * stmt_str, map<string, string*> * nameTable)  {
	string label("lp" + to_string(lCnt) + ":\n");
	*stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + label;

	label = string("d" + to_string(lCnt) );
	e->assem(stmt_str, &label);

  *stmt_str = *stmt_str + "\t";
	s->assem(stmt_str, nameTable);
	*stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + "\tb lp" + to_string(lCnt) + "\nd" + to_string(lCnt) + ":\n";
	
	lCnt++;
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
//string * PrintLineExp::assem(){
void PrintLineExp::assem(string * stmt_str, map<string, string*> * nameTable){
	// piazza says never print a boolean
	// need to put the format string somewhere
	string e_str("");
	e->assem(&e_str, NULL);
	string s("ldr r0, =");
	s = e_str + "\t" + s + PRINTLN_EXP + "\n\tbl printf\n";
	*stmt_str = *stmt_str + s.substr(1);
	//return new string( s.substr(1) );
	//return new string("PRINT LINE EXP");
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
void PrintLineString::assem(string * stmt_str, map<string, string*> * nameTable) {
	// Adding \n to string here and creating a mem leak XD
	string * str = new string(*(s->str) + "\\n");
	//cout << str->c_str() << endl;
	s->str = str;

	s->assem(); // adds stuff to text section + creates label
	branchPrint(s->label, stmt_str);
	//return branchPrint(s->label, );
	//return new string("PRINT LINE STRING");
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
//string * PrintExp::assem(){
void PrintExp::assem(string * stmt_str, map<string, string*> * nameTable){
	// piazza says never print a boolean
	string e_str("");
	e->assem(&e_str, NULL);
	string s("ldr r0, =");
	s = e_str + "\t" + s + PRINT_EXP + "\n\tbl printf\n";
	*stmt_str = *stmt_str + s.substr(1);
	//s = s + PRINT_EXP + "\n\tbl printf";
	//return new string( s.substr(1) );
	//return new string("PRINT EXP");
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
//string * PrintString::assem(){
void PrintString::assem(string * stmt_str, map<string, string*> * nameTable){
	s->assem();
	branchPrint(s->label, stmt_str);
	//return branchPrint(s->label);
	//return new string("PRINT STRING");
}

// Assign //
void Assign::traverse() {
	i->traverse();
	expErr = false;
	e->traverse();
	if (expErr) { e->reportError(); return; }
	TableNode * t = i->lowestT;
	// Now we gonna check that type
	while (t->table->find(*(i->id)) == t->table->end() ) {
		// ok symbol not there REPORT
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
void Assign::assem(string * stmt_str, map<string, string*> * nameTable) {
	string exp_str = "";
	e->assem(&exp_str, NULL);
	*stmt_str = *stmt_str + exp_str.substr(1) + "\n";

	map<string, string*> * table = nameTableStack.back();
	if (table->find(*(i->id)) == table->end() 
			&& nameTableStack.size() > 1) {
		//cout << "table working" << endl;
		table = nameTableStack[nameTableStack.size() - 2];
	}
	string * label = (*table)[*(i->id)];
	if (label) {
		*stmt_str = *stmt_str + "\tldr r0, =" + *label + "\n";
		*stmt_str = *stmt_str + "\tstr r1, [r0]" +  "\n";
	}
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
void ReturnStatement::assem(string * stmt_str, map<string, string*> *nT){
	e->assem(stmt_str, NULL);
	*stmt_str = *stmt_str + "\n\tmov r0, r1\n\tpop {pc}\n";
}

// StatementList //
void StatementList::traverse() {
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		(*s)->traverse();
	}
	PRINTDEBUGTREE("StatementList");
}
void StatementList::setTableNodes(TableNode * n) {
	// CHECK - add code to set the contents of each statement lowestT
	// DONE - make virtual method for this in each statement
	// BOOYA - make function for each statement
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
//string * StatementList::assem() {
void StatementList::assem(string * stmt_str, map<string, string*> * nameTable) {
	string str("");
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		//str = str + "\t" + (*s)->assem()->c_str() + "\n";
		str = str + "\t";
		(*s)->assem(&str, nameTable);
		str = str + "\n";
	}
	*stmt_str = *stmt_str + str.substr(1);
	//return new string(str.substr(1));
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
	//cout << r1 << " + " << r2 << endl;
	return r1 || r2;
}
void Or::assem(string * exp_str, string * branchLabel){
	string label1( "e_or_S" + to_string(expCnt++) );
	string label2( "e_or_E" + to_string(expCnt++) );
	
	if (!branchLabel) {
		e1->assem(exp_str, NULL); //&label);
		*exp_str = *exp_str + "\tpush {r1}\n";
	} else {
		e1->assem(exp_str, &label1); //&label);
		*exp_str = *exp_str + "\tb " + label2 + "\n" + label1 + ":\n";
	}
	
	e2->assem(exp_str, branchLabel);
	if (!branchLabel) {
		*exp_str = *exp_str + "\tpop {r2}\n\torr r1, r1, r2\n";
	} else {
		*exp_str = *exp_str + label2 + ":\n";
	}
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
	//cout << r1 << " && " << r2 << endl;
	return r1 && r2;
}
void And::assem(string * exp_str, string * branchLabel){
	string label( "e" + to_string(expCnt++) );
	e1->assem(exp_str, branchLabel);
	if (!branchLabel) {
		*exp_str = *exp_str + "\tpush {r1}\n";
	}
	e2->assem(exp_str, branchLabel);//&label);
	if (!branchLabel) {
		*exp_str = *exp_str + "\tpop {r2}\n\tand r1, r1, r2\n";
	} else {
		*exp_str = *exp_str + label + ":\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 == r2;
}
void Equal::assem(string * exp_str, string * branchLabel){
	e1->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpush {r1}\n";
	e2->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
	if (branchLabel) {
		*exp_str = *exp_str + "\tbne " + *branchLabel + "\n";
	} else {
		*exp_str = *exp_str + "\tmov r1, #0\n\tmoveq r1, #1\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 != r2;
}
void NotEqual::assem(string * exp_str, string * branchLabel){
	e1->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpush {r1}\n";
	e2->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
	if (branchLabel) {
		*exp_str = *exp_str + "\tbeq " + *branchLabel + "\n";
	}	else {
		*exp_str = *exp_str + "\tmov r1, #0\n\tmovne r1, #1\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 < r2;
}
void Lesser::assem(string * exp_str, string * branchLabel){
	e1->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpush {r1}\n";
	e2->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
	if (branchLabel) {
		*exp_str = *exp_str + "\tbge " + *branchLabel + "\n";
	} else {
		*exp_str = *exp_str + "mov r1, #0\n\tmovlt r1, #1\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 > r2;
}
void Greater::assem(string * exp_str, string * branchLabel){
		e1->assem(exp_str, NULL); // has to b int
		*exp_str = *exp_str + "\tpush {r1}\n";
		e2->assem(exp_str, NULL); // has to b int
		*exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n\t";
	if (branchLabel) {
		*exp_str = *exp_str + "ble " + *branchLabel + "\n";
	} else {
		*exp_str = *exp_str + "mov r1, #0\n\tmovgt r1, #1\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 <= r2;
}
void LessEqual::assem(string * exp_str, string * branchLabel){
	e1->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpush {r1}\n";
	e2->assem(exp_str, NULL); // has to b int
	*exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
	if (branchLabel) {
		*exp_str = *exp_str + "\tbgt " + *branchLabel + "\n";
	} else {
		*exp_str = *exp_str + "\tmov r1, #0\n\tmovle r1, #1\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 >= r2;
}
void GreatEqual::assem(string * exp_str, string * branchLabel){
		e1->assem(exp_str, NULL); // has to b int
		*exp_str = *exp_str + "\tpush {r1}\n";
		e2->assem(exp_str, NULL); // has to b int
		*exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
	if (branchLabel) {
		*exp_str = *exp_str + "\tblt " + *branchLabel + "\n";
	} else {
		*exp_str = *exp_str + "\tmov r1, #0\n\tmovge r1, #1\n";
	}
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
	//cout << r1 << " + " << r2 << endl;
	return r1 + r2;
}
void Add::assem(string * exp_str, string * branchLabel){
	//if (dynamic_cast<LitInt *>( e1 )) {
	if (isIntLiteral(e1)) {
		std::string lit_string("");
		getIntLiteral (e1, &lit_string);
		//LitInt * expr = (LitInt *) e1;
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tadd r1, r1, " + lit_string + "\n";
		//*exp_str = *exp_str + "\tadd r1, r1, #" + to_string(expr->i->i) + "\n";
	//} else if (dynamic_cast<LitInt *>( e2 )) {
	} else if (isIntLiteral( e2 )) {
		//LitInt * expr = (LitInt *) e2;
		std::string lit_string("");
		getIntLiteral (e2, &lit_string);
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tadd r1, r1, " + lit_string + "\n";
	} else {
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpush {r1}\n";
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpop {r2}\n\tadd r1, r2, r1\n";
	}
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
	//cout << r1 << " - " << r2 << endl;
	return r1 - r2;
}
void Subtract::assem(string * exp_str, string * branchLabel){
	if ( isIntLiteral( e2 ) ) {
		std::string lit_string("");
		getIntLiteral (e2, &lit_string);
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tsub r1, r1, " + lit_string + "\n";
	} else if ( isIntLiteral( e1 ) ) {
		std::string lit_string("");
		getIntLiteral (e1, &lit_string);
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tmov r2, " + lit_string + "\n\tsub r1, r2, r1\n";
	} else {
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpush {r1}\n";
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpop {r2}\n\tsub r1, r2, r1\n";
	}
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
	//cout << r1 << " / " << r2 << endl;
	return r1 / r2;
}
void Divide::assem(string * exp_str, string * branchLabel) {
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpush {r1}\n";
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpop {r2}\n\tmov r0, #0\n";
		*exp_str = *exp_str + "\tmov r4, #0\n\tcmp r2, #0\n\tbge div" + to_string(lCnt) + "\n\tmov r4, #1\n\trsb r2, #0\n";
		*exp_str = *exp_str + "div" + to_string(lCnt) + ":\n\tcmp r2, r1\n\tblt d" + to_string(lCnt) + "\n";
		*exp_str = *exp_str + "\tsub r2, r2, r1\n\tadd r0, r0, #1\n\tb div" + to_string(lCnt) + "\n";
		*exp_str = *exp_str + "d" + to_string(lCnt) + ":\n\tmov r1, r0\n";
		*exp_str = *exp_str + "\tcmp r4, #1\n\trsbeq r1, #0\n";
		lCnt++;
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
	//cout << r1 << " * " << r2 << endl;
	return r1 * r2;
}
void Multiply::assem(string * exp_str, string * branchLabel){
	//if (dynamic_cast<LitInt *>( e1 ) && dynamic_cast<LitInt *>( e2 )) {
	if (isIntLiteral( e1 ) && isIntLiteral( e2 )) {
		// Both are positive int literals
		//LitInt * expr1 = (LitInt *) e1;
		//LitInt * expr2 = (LitInt *) e2;
		string lit_string("");
		getIntLiteral (e1, &lit_string);
		*exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
		lit_string = string("");
		getIntLiteral (e2, &lit_string);
		*exp_str = *exp_str + "\tmov r1, " + lit_string + "\n";
		*exp_str = *exp_str + "\tmul r1, r2, r1\n";
	//} else if (dynamic_cast<LitInt *>( e1 )) { // op1 is a literal
	} else if (isIntLiteral( e1 )) { // op1 is a literal
		//LitInt * expr1 = (LitInt *) e1;
		std::string lit_string("");
		getIntLiteral (e1, &lit_string);
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
		*exp_str = *exp_str + "\tmul r1, r2, r1\n";
	//} else if (dynamic_cast<LitInt *>( e2 )) { // op 2 is a literal
	} else if (isIntLiteral( e2 )) { // op 2 is a literal
		//LitInt * expr2 = (LitInt *) e2;
		std::string lit_string("");
		getIntLiteral (e2, &lit_string);
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
		*exp_str = *exp_str + "\tmul r1, r2, r1\n";
	} else { // Neither are int pos int literals
		e1->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpush {r1}\n";
		e2->assem(exp_str, NULL);
		*exp_str = *exp_str + "\tpop {r2}\n\tmul r1, r2, r1\n";
	}
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
void Not::assem(string * exp_str, string * branchLabel) {
	if (dynamic_cast<True *>( e )){ // if !true = false 
		// False always branches
		if (branchLabel) {
			*exp_str = *exp_str + "\tb " + *branchLabel + "\n";
		} else {
			*exp_str = *exp_str + "\tmov r0, #0\n";
		}
	} else if (dynamic_cast<False *>( e )) {	// if !false = true
		// Do Nothing - true will always execute the first part of the statement
		if (!branchLabel){
			*exp_str = *exp_str + "\tmov r0, #1\n";
		}
	} else if (dynamic_cast<Not *>( e )) { // dont do anything with !!
		Not * expr = (Not * ) e;
		expr->e->assem(exp_str, branchLabel);
	} else if ( dynamic_cast< ParenExp * >( e ) ) { // if !()
		Exp * nExp = NULL;
		ParenExp * p = (ParenExp *) e;
		nExp = p->e;
		while (dynamic_cast<ParenExp *>( nExp )) {
				ParenExp * p = (ParenExp *) nExp;
				nExp = p->e;
		} 
		if (dynamic_cast<Not *>(nExp)) {
			Not * expr = (Not * ) nExp;
			expr->e->assem(exp_str, branchLabel);
		} else if (dynamic_cast<False *>( nExp )) {
			// Do Nothing - true will always execute the first part of the statement
			if (!branchLabel){
				*exp_str = *exp_str + "\tmov r0, #1\n";
			}
		} else if (dynamic_cast<True *>( nExp )) {
			// False always branches
			if (branchLabel) {
				*exp_str = *exp_str + "\tb " + *branchLabel + "\n";
			} else {
				*exp_str = *exp_str + "\tmov r0, #0\n";
			}
		} else {
			if (!branchLabel) {
				nExp->assem(exp_str, branchLabel);
				*exp_str = *exp_str + "\teor r1, r1, #1\n";
			} else {
				string label("not" + to_string(expCnt++));
				nExp->assem(exp_str, &label);
				*exp_str = *exp_str + "\tb " + *branchLabel + "\n"+ label + ":\n";
			}
		}
	} else {
		if (!branchLabel) {
			e->assem(exp_str, NULL);
			*exp_str = *exp_str + "\teor r1, r1, #1\n";
		} else {
			string label("not" + to_string(expCnt++));
			e->assem(exp_str, &label);
			*exp_str = *exp_str + "\tb " + *branchLabel + "\n"+ label + ":\n";
		}
	}

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
void Pos::assem(string * exp_str, string * branchLabel) {
	e->assem(exp_str, NULL);
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
void Neg::assem(string * exp_str, string * branchLabel) {
	if (dynamic_cast<LitInt *>( e )) {
		LitInt * expr = (LitInt *) e;
		*exp_str = *exp_str + "\tmov r1, #-"+ to_string(expr->i->i) + "\n";
	} else if (dynamic_cast<Neg *>( e )) {
		Neg * expr = (Neg *) e;
		expr->e->assem(exp_str, NULL);
	} else if (dynamic_cast< ParenExp * >( e )
							|| dynamic_cast< Pos * >( e ) ) { // eliminate paren

		Exp * nExp = NULL;
		if (dynamic_cast< ParenExp * >( e )) {
			ParenExp * p = (ParenExp *) e;
			nExp = p->e;
		} else if (dynamic_cast< Pos * >( e )) { 
			Pos * p = (Pos *) e;
			nExp = p->e;
		} else {
			cout << "OPPS! This is bad." << endl;
		}

		while (dynamic_cast<ParenExp *>( nExp ) 
				|| dynamic_cast<Pos *>( nExp )) {
			if (dynamic_cast< ParenExp * >( nExp )) {
				ParenExp * p = (ParenExp *) nExp;
				nExp = p->e;
			} else if (dynamic_cast< Pos * >( nExp )) {
				Pos * p = (Pos *) nExp;
				nExp = p->e;
			} else {
				cout << "OPPS! This is bad." << endl;
			}
		} 
		// There was an int in the mess
		if (dynamic_cast<LitInt *>(nExp)) {
			LitInt * expr = (LitInt *) e;
			*exp_str = *exp_str + "\tmov r1, #-"+ to_string(expr->i->i) + "\n";
		} else if (dynamic_cast<Neg *>(nExp)) { // it was another neg
			Neg * expr = (Neg *) nExp;
			expr->e->assem(exp_str, NULL);
		} else { // something else
			nExp->assem(exp_str, NULL);
			*exp_str = *exp_str + "\trsb r1, r1, #0\n";
		}
	} else {
		e->assem(exp_str, NULL);
		*exp_str = *exp_str + "\trsb r1, r1, #0\n";
		//*exp_str = *exp_str + "\tmov r2, #-1\n\tmul r1, r1, r2\n";
	}
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
void ParenExp::assem(string * exp_str, string * branchLabel) {
	e->assem(exp_str, branchLabel);
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
	//fprintf(stderr, "Int: %d\n", i->i);
	return i->i;
}
void LitInt::assem(string * exp_str, string * branchLabel) {
	*exp_str = *exp_str + "\tmov r1, #"+ to_string(i->i) + "\n";
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
void True::assem(string * exp_str, string * branchLabel){
	// Do Nothing - true will always execute the first part of the statement
	if (branchLabel == NULL){
		*exp_str = *exp_str + "\tmov r1, #1\n";
	}
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
void False::assem(string * exp_str, string * branchLabel){
	// False always branches
	if (branchLabel == NULL){
		*exp_str = *exp_str + "\tmov r1, #0\n";
	} else {
		*exp_str = *exp_str + "\tb " + *branchLabel + "\n";
	}
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
void ExpObject::assem(string * exp_str, string * branchLabel) {
	o->assem(exp_str, branchLabel);
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
void ObjectMethodCall::assem(string * exp_str, string * branchLabel) {
	if ( dynamic_cast<NewIdObj * >(o) ) {
		NewIdObj * obj = (NewIdObj * ) o;
		*exp_str = *exp_str + "\tbl " + *(obj->i->id) + *(i->id) + "\n";
		*exp_str = *exp_str + "\tmov r1, r0\n";
	} else if ( dynamic_cast<IdObj * >(o) ) {
		IdObj * obj = (IdObj * ) o;
		*exp_str = *exp_str + "\tbl " + *(obj->i->id) + *(i->id) + "\n";
		*exp_str = *exp_str + "\tmov r1, r0\n";
	}
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
void IdObj::assem(string * exp_str, string * branchLabel){
	map<string, string*> * table = nameTableStack.back();
	if (table->find(*(i->id)) == table->end() 
			&& nameTableStack.size() > 1) {
		//cout << "table working" << endl;
		table = nameTableStack[nameTableStack.size() - 2];
	}
	string * label = (*table)[*(i->id)];
	if (label) {
		*exp_str = *exp_str + "\tldr r0, =" + *label + "\n";
		*exp_str = *exp_str + "\tldr r1, [r0]" + "\n";
	}
	//*stmt_str = *stmt_str + "\tstr r1, [r0]" +  "\n";
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
void NewTypeObj::assem(string * exp_str, string * branchLabel) {
	map<string, string*> * table = nameTableStack.back();
	if (table->find(*(i->id)) == table->end() 
			&& nameTableStack.size() > 1) {
		//cout << "table working" << endl;
		table = nameTableStack[nameTableStack.size() - 2];
	}
	string * label = (*table)[*(i->id)];
	if (label) {
		*exp_str = *exp_str + "\tldr r0, =" + *label + "\n";
		*exp_str = *exp_str + "\tldr r1, [r0]" + "\n";
	}
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
	e->setTable(t);
}

// ExpRest //
void ExpRest::traverse() {
	e->traverse();
	PRINTDEBUGTREE("ExpRest");
} 
void ExpRest::setTable(TableNode * t){
	lowestT = t;
	e->setTable(t);
}

// Ident //
void Ident::traverse() {
	//fprintf(stderr, "At ID Leaf: %s\n", id->c_str());
}

// StringLiteral //
void StringLiteral::traverse() {
	//fprintf(stderr, "At StringLiteral Leaf: %s\n", str->c_str());
}
void StringLiteral::assem() {
	programRoot->textSection->push_back(new string("str" + to_string(strCnt) + ": .asciz \"" + str->c_str() + "\""));
	label = new string("str" + to_string(strCnt++));
}

// IntLiteral //
void IntLiteral::traverse() {
	//fprintf(stderr, "At Leaf: %d\n", i);
}
