
/*
 *
 * miniCompEval
 * Sonya Schriner
 * File: ARMv7.hpp
 *
 */

#ifndef ARMV7_HPP
#define ARMV7_HPP

#include "TreeNode.hpp"
#include <vector>

using std::string;
using std::map;
using std::vector;

namespace ARMv7 {

int ifCnt = 0;
int lCnt = 0;
int strCnt = 0;
int expCnt = 0;
int regCnt = 0;

struct AssemContext {
	//TODO(ss) ofstream * assemStream;
	vector<string * > * dataSection;
	vector<string * > * textSection;
	

	//TODO(ss) move me
	//ClassDeclSimple 
	//map<string|ClassDecl, map<string, string*>> nameTable = new map<string, string*>;
	//map<string|ClassDecl, map<string, string*>> typeTable = new map<string, string*>;
	//MethodDecl
	//map<string, string*> * nameTable = nullptr; // id -> symRegLabel
	//map<string, string*> * typeTable = nullptr; // id -> symRegLabel

	vector< map<string, string*> * > nameTableStack;
	vector< map<string, string*> * > typeTableStack;

};

void branchPrint(string * str, string * stmt_str) {
	string s("ldr r0, =");
	s = s + str->c_str() + "\n\tbl printf\n";
	*stmt_str = *stmt_str + s;
}

bool isIntLiteral(Exp * e) {
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
		*s = *s + "#" + to_string(expr->i);
		return;
	}

	int negCnt = 0;
	while (dynamic_cast<ParenExp *>( e ) || dynamic_cast<Neg *> ( e )
			|| dynamic_cast<LitInt *>( e ) || dynamic_cast<Pos *> ( e )) {
		if (ParenExp * expr = dynamic_cast<ParenExp *>( e )) { // ignore parentheses
			e = expr->e;
		} else if (Pos * expr = dynamic_cast< Pos * >( e )) {
			e = expr->e;
		} else if (Neg * expr = dynamic_cast< Neg * > ( e )) { // count the negative
			negCnt++;
			e = expr->e;
		} else if (LitInt * expr = dynamic_cast<LitInt *>( e )) {
			if (negCnt % 2) {
				*s = *s + "#-" + to_string(expr->i);
			} else {
				*s = *s + "#" + to_string(expr->i);
			}

			// stop when you hit the literal
			return;
		}
	}
	cerr << "Error processing integer literal for assembly generation, lineno: " << e->lineRecord <<endl;
}

} // ARMv7
#endif
