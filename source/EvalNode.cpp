
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

//TODO(ss): void VarDecl::evaluate() {
// Account For type
//}

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

//TODO(ss)void Assign::evaluate() {
//  TypeCheck
//	cout << *(s->str);
//}

//TODO(ss)void IndexAssign::evaluate() {}

//TODO(ss) void ReturnStatement::evaluate(TableNode * t) {}

void StatementList::evaluate() {
	for (auto s = sVector->begin(); s < sVector->end(); s++) {
		(*s)->evaluate();
	}
}

//TODO(ss)void SingleIndex::evaluate() {}

//TODO(ss)void MultipleIndices::evaluate() {}

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

//TODO(ss)void ArrayAccess::evaluate() {}
//TODO(ss)void ArrayAccessLength::evaluate() {}

/* list of "exp" or "exp , exp, ..." */
//TODO(ss) void ExpList::evaluate() { // add erl or e to table }

int LitInt::evaluate() {
	return i->i;
}

int True::evaluate() {
	return 1;
}

int False::evaluate() {
	return 0;
}

#endif
