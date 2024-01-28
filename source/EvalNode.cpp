
/*
 * miniCompEval
 * Sonya Schriner
 * File: TreeNode.cpp
 *
 */

#include "TreeNode.hpp"

using namespace std;

#ifndef ASSEM

void Program::evaluate() {
	  if (programTypeError) return;
		  m->evaluate();
}

void MainClass::evaluate() {
	  s->evaluate();
}

//TODO(ss): void ClassDeclList::evaluate() {}
//TODO(ss): void ClassDecl::evaluate() {}

//TODO(ss): void ClassDeclExtends::evaluate() {}

//TODO(ss): void VarDeclList::evaluate() {}
//TODO(ss): void VarDecl::evaluate() { Account for type }

//TODO(ss): void MethodDecl::evaluate() {}
//TODO(ss): void MethodDeclList::evaluate() {}

//TODO(ss): void FormalList::evaluate() {}
//TODO(ss): void FormalRestList::evaluate() {}
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

//TODO(ss)void Assign::evaluate() { /*  TypeCheck */ cout << *(s->str); }

//TODO(ss)(Array)void IndexAssign::evaluate() {}

void ReturnStatement::evaluate() {
	e->evaluate();
	//TODO(ss) Fix table;
}

void StatementList::evaluate() {
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		(*s)->evaluate();
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

//TODO(ss)void ExpObject::evaluate() {}
int ObjectMethodCall::evaluate() {
	// look up the method
	// place the result somewhere
	if (dynamic_cast<NewTypeObj>(o)) {
		// look up the method in the table and traverse
	}
	return 0;
}
//TODO(ss)void IdObj::evaluate() {}
//TODO(ss)void ThisObj::evaluate() {}
int NewIdObj::evaluate() {
	// create an obj and add it to a table or return the addr
	return 0;
}
//TODO(ss)void NewTypeObj::evaluate() {}

/* list of "exp" or "exp , exp, ..." */
//TODO(ss) void ExpList::evaluate() { // add erl or e to table }

#endif
