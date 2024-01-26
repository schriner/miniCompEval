
/*
 *
 * CS352: Project 3 
 * Sonya Schriner
 * File: TreeNode.hpp
 *
 */

#include <vector>
#include <iostream>

#include "TableNode.hpp"
#define PRINTDEBUGTREE(x) //fprintf(stderr, "At Node: %s\n", (x));
#define PRINTLN_EXP "pln"
#define PRINT_EXP "p"

using namespace std;

extern int yylineno;
extern bool programTypeError;

// Forward Class Declarations 
class MainClass; class ClassDeclList; class ClassDecl;
class VarDeclList; class VarDecl; 
class MethodDeclList; class MethodDecl;  
class FormalList; class FormalRestList; class FormalRest;
class PrimeType; class Type;
class StatementList; class Statement;
class Index; class Exp; class Object;
class ExpList; class ExpRestList; class ExpRest;
class Ident; class IntLiteral; class StringLiteral;

// Parent Abstract Class of Everything
class TreeNode {
	public: 
		TableNode * lowestT = NULL;
		Data * data = NULL; 
		int lineRecord = -1;
		virtual void traverse() = 0; 
		void reportError() {
			programTypeError = true;
			fprintf(stderr, "Type Violation in Line %d\n", lineRecord);
		}
};

class Program : public TreeNode {
	public:
		MainClass * m = NULL;
		ClassDeclList * c = NULL;
		vector<string * > * dataSection = NULL;
		vector<string * > * textSection = NULL;
		Program (MainClass * m, ClassDeclList * c) {
			this->m = m;
			this->c = c;
			dataSection = new vector<string * >;
			textSection = new vector<string * >;
			textSection->push_back(new string(string(PRINTLN_EXP) + ": .asciz \"%d\\n\""));
			textSection->push_back(new string(string(PRINT_EXP) + ": .asciz \"%d\""));
			lineRecord = yylineno;
		}
		void traverse(); 
		void evaluate();
		void assem();
};

class MainClass : public TreeNode {
	public:
		Ident * i1 = NULL;
		Ident * i2 = NULL;
		Statement * s = NULL;
		vector<string * > * instr = NULL;
		MainClass (Ident * i1, Ident * i2, Statement * s) {
			this->i1 = i1;
			this->i2 = i2;
			this->s = s;
			instr = new vector<string * >;
			lineRecord = yylineno;
		}
		void traverse();
		void evaluate();
		void assem();
};

class ClassDeclList : public TreeNode {
	public:
		vector<ClassDecl * > * cdVector = NULL;
		vector<string * > * instr = NULL;
		ClassDeclList(ClassDecl * c) { 
			cdVector = new vector<ClassDecl * >;
			cdVector->push_back(c);
			instr = new vector<string * >;
			lineRecord = yylineno;
		}
		// Add an element to the 
		void append(ClassDecl * c) {
			cdVector->push_back(c);
		}
		void traverse();
		void assem();
};

/* Abstract class ClassDecl Start */
class ClassDecl : public TreeNode {
	public:
		virtual void assem() = 0;
};
class ClassDeclSimple : public ClassDecl {
	public:
		Ident * i = NULL;
		VarDeclList * v = NULL;
		MethodDeclList * m = NULL;
		map<string, string*> * nameTable = NULL; // id -> symRegLabel
		map<string, string*> * typeTable = NULL; // id -> symRegLabel

		ClassDeclSimple (Ident * i, VarDeclList * v,  MethodDeclList * m) { 
			this->i = i;
			this->v = v;
			this->m = m;
			lineRecord = yylineno;
			nameTable = new map<string, string*>;
			typeTable = new map<string, string*>;
		}
		void traverse();
		virtual void assem();
};
class ClassDeclExtends : public ClassDecl {
	public:
		Ident * i1 = NULL; Ident * i2 = NULL;
		VarDeclList * v = NULL;
		MethodDeclList * m = NULL;
		ClassDeclExtends (Ident * i1, Ident * i2, VarDeclList * v,  MethodDeclList * m) { 
			this->i1 = i1; this->i2 = i2;
			this->v = v;
			this->m = m;
			lineRecord = yylineno;
		}
		void traverse();
		virtual void assem();
};
/* Abstract class ClassDecl End */

class VarDecl : public TreeNode {
	public: 
		Type * t = NULL;
		Ident * i = NULL;
		string * label = NULL;
		VarDecl(Type * t, Ident * i) {
			this->t = t;
			this->i = i;
			lineRecord = yylineno;
		} 
		void traverse();
		void setTable(TableNode * ta);
		void assem(map<string, string*> *, map<string, string*> *);
};

class VarDeclList : public TreeNode { 
	public:
		vector<VarDecl * > * vdVector = NULL;
		VarDeclList(VarDecl * v) {
			vdVector = new vector<VarDecl * >;
			vdVector->push_back(v);
			lineRecord = yylineno;
		}
		void append(VarDecl * v) {
			vdVector->push_back(v);
		}
		void setTableNodes(TableNode * n) {
			lowestT = n;
			for (auto v = vdVector->begin(); v < vdVector->end(); v++) {
				(*v)->setTable(n);
			}
		}
		void traverse();
		void assem(map<string, string*> *, map<string, string*> *);
		//void assem();
};

class MethodDecl : public TreeNode {
	public:
		Type * t = NULL;
		Ident * i = NULL;
		FormalList * f = NULL;
		VarDeclList * v = NULL; 
		StatementList * s = NULL;
		Exp * e = NULL;
		map<string, string*> * nameTable = NULL; // id -> symRegLabel
		map<string, string*> * typeTable = NULL; // id -> symRegLabel

		MethodDecl(Type * t, Ident * i, FormalList * f, VarDeclList * v, StatementList * s, Exp * e) {
			this->t = t;
			this->i = i;
			this->f = f;
			this->v = v; 
			this->s = s;
			this->e = e;
			nameTable = new map<string, string*>;
			typeTable = new map<string, string*>;
			lineRecord = yylineno;
		} 
		void traverse();
		void assem(string * objName);
};

class MethodDeclList : public TreeNode {
	public:
		vector<MethodDecl * > * mdVector = NULL;
		MethodDeclList (MethodDecl * m) { 
			mdVector = new vector<MethodDecl * >;
			mdVector->push_back(m);
			lineRecord = yylineno;
		}
		void append(MethodDecl * m) {
			mdVector->push_back(m);
		}
		void setTableParentNodes(TableNode * p);/*{
			for (auto m = mdVector->begin(); m < mdVector->end(); m++) {	
				(*m)->lowestT->parent = p;
				if (data) {
					if ( p->table->find( string(*((*m)->i->id)) ) == p->table->end() ) {
						(*p->table)[string(*((*m)->i->id))] = data;
					} else {
						lowestT->appendBadMethodDecl(*m);
					}	
				}
			}
		}*/
		void traverse();
		virtual void assem(string * objName);
};

class FormalList : public TreeNode {
	public:
		Type * t = NULL;
		Ident * i = NULL;
		FormalRestList * f = NULL;
		FormalList(Type * t, Ident * i, FormalRestList * f){
			this->t = t;
			this->i = i;
			this->f = f;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};

class FormalRestList : public TreeNode {
	public:
		vector<FormalRest * > * frVector = NULL;
		FormalRestList(FormalRest * f) {
			frVector = new vector<FormalRest * >;
			frVector->push_back(f);
			lineRecord = yylineno;
		}
		void append(FormalRest * f) {
			frVector->push_back(f);
		}
		void traverse();
		void setTable(TableNode * t);
};

class FormalRest : public TreeNode {
	public:
		Type * t = NULL;
		Ident * i = NULL;
		FormalRest(Type * t, Ident * i){
			this->t = t;
			this->i = i;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};

/* Begin Abstract Class PrimeType */
class PrimeType : public TreeNode {
	public:
		virtual void setTable(TableNode * t) = 0;
};  
class IntType : public PrimeType { 
	public: 
		IntType() { lineRecord = yylineno; } 
		void traverse();
		void setTable(TableNode * t);
};
class BoolType : public PrimeType { 
	public: 
		BoolType() { lineRecord = yylineno; } 
		void traverse();
		void setTable(TableNode * t);
};
class IdentType : public PrimeType { 
	public: 
		Ident * i = NULL;
		IdentType(Ident * i) {
			this->i = i;
			lineRecord = yylineno;
		} 
		void traverse();
		void setTable(TableNode * t);
};
/* End Abstract Class PrimeType */

/* Abstract Class Type */
class Type : public TreeNode {
	public:
		virtual void setTable(TableNode * t) = 0;
};
class TypePrime : public Type { 
	public:
		PrimeType * p = NULL;
		TypePrime(PrimeType * p){
			this->p = p;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
class TypeIndexList : public Type {
	public:
		Type * t = NULL;
		TypeIndexList(Type * t) {
			this->t = t;
			lineRecord = yylineno;
		}
		TypeIndexList * getLastNull() { //for the type_bracket_list non-term
			TypeIndexList * node = this;
			// Find the NULL statement
			while (node->t != NULL){
				node = (TypeIndexList *) node->t;
			}
			return node;
		}
		void traverse();
		void setTable(TableNode * t);
};
/* End Class Type */

/* Abstract Class Statement Start */
class Statement : public TreeNode { 
	public:
		virtual void setTable(TableNode * t) = 0;
		virtual void evaluate() {
			cout << "\nStatement : D" << endl;
		}
		//virtual string * assem(){
		virtual void assem(string * stmt_str, map<string, string*> *) {
			cout << "\nYOLO" << *stmt_str << endl;
			//return new string("");
		}
};
class BlockStatements : public Statement {
	// For: { StatementList }  -- SL = NULL for: { } 
	public:
		StatementList * s = NULL;
		BlockStatements(StatementList * s) {
			this->s = s;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		void assem(string * stmt_str, map<string, string*> *);
};
class IfStatement : public Statement {
	// For: if (e) s_if else s_el 
	public:
		Exp * e = NULL;
		Statement * s_if = NULL; Statement * s_el = NULL;
		IfStatement( Exp * e, Statement * s_if, Statement * s_el ) {
			this->s_if = s_if; this->s_el = s_el;
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		void assem(string * stmt_str, map<string, string*> *);
};
class WhileStatement : public Statement {
	// For: while (e) s
	public:
		Exp * e = NULL;
		Statement * s = NULL;
		WhileStatement(Exp * e, Statement * s){
			this->e = e;
			this->s = s;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		void assem(string * stmt_str, map<string, string*> *);
};
class PrintLineExp : public Statement {
	// For: .println( e )
	public:
		Exp * e = NULL;
		PrintLineExp(Exp * e) {
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		//string * assem();
		void assem(string * stmt_str, map<string, string*> *);
};
class PrintLineString : public Statement {
	// For: .println( s )
	public:
		StringLiteral * s = NULL;
		PrintLineString (StringLiteral * s) {
			this->s = s;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		//string * assem();
		void assem(string * stmt_str, map<string, string*> *);
};
class PrintExp : public Statement {
	// For: .print( e )
	public:
		Exp * e = NULL;
		PrintExp (Exp * e) {
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		//string * assem();
		void assem(string * stmt_str, map<string, string*> *);
};
class PrintString : public Statement {
	// For: .print( s )
	public:
		StringLiteral * s = NULL;
		PrintString (StringLiteral * s) {
			this->s = s;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
		//string * assem();
		void assem(string * stmt_str, map<string, string*> *);
};
class Assign : public Statement {
	// For: i = e
	public:
		Ident * i = NULL;
		Exp * e = NULL;
		Assign(Ident * i, Exp * e){
			this->i = i;
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
		// TODO: void evaluate();
};
class IndexAssign : public Statement {
	// For: i ind = e
	public:
		Ident * i = NULL;
		Index * ind = NULL;
		Exp * e = NULL;
		IndexAssign(Ident * i, Index * ind, Exp * e){
			this->i = i;
			this->ind = ind;
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
class ReturnStatement : public Statement {
	// For: return e 
	public:
		Exp * e = NULL;
		ReturnStatement(Exp * e){
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
		// TODO: void evaluate();
};
/* Abstract Class Statement End */

class StatementList : public TreeNode {
	public:
		vector<Statement * > * sVector = NULL;
		StatementList(Statement * s) {
			sVector = new vector<Statement * >;
			sVector->push_back(s);
			lineRecord = yylineno;
		}
		void append(Statement * s) {
			sVector->push_back(s);
		}
		void setTableNodes(TableNode * n);
		void traverse();
		void evaluate();
		//string * assem();
		void assem(string * stmt_str, map<string, string*> *);
};

/* Begin Abstract Class Index */
class Index : public TreeNode {
	public:
		virtual void setTable(TableNode * t) = 0;
}; 
class SingleIndex : public Index {
	public:
		Exp * e = NULL;
		SingleIndex(Exp * e){
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
class MultipleIndices : public Index {
	public:
		Index * ind = NULL;
		Exp * e = NULL;
		MultipleIndices(Index * ind, Exp * e){
			this->ind = ind;
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
/* End Abstract Class Index */

/* Start Expression Abstract Classes */
class Exp : public TreeNode {
	public:
		virtual void setTable(TableNode * t) {
			fprintf(stderr, "PRINT\n");
		}
		// for bool values evaluate stuff to 0 and 1
		virtual int evaluate() {
			cout << "\nOOPS!! Expr function not implemented" << endl;
			return 1;
		} 
		/*virtual void assem(string * exp_str) {
			*exp_str = *exp_str + "\n\tNEXT EXPRESSION ASSEM CODE";
		}*/
		virtual void assem(string * exp_str, string * branchLabel) {
			*exp_str = *exp_str + "\n\tNEXT EXPRESSION ASSEM CODE Huzzah!";
		} 
}; 
	    // Start Binary op
class BinExp : public Exp { 
	public:
		Exp * e1 = NULL; Exp * e2 = NULL; 
		void setTable(TableNode * t);
};
class Or : public BinExp { 
	public:
		Or(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		} 
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel);
};
class And : public BinExp {
	public:
		And(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		} 
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel);
};
class Equal : public BinExp {
	public:
		Equal(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class NotEqual : public BinExp {
	public:
		NotEqual(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Lesser : public BinExp {
	public:
		Lesser(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Greater : public BinExp {
	public:
		Greater(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class LessEqual : public BinExp {
	public:
		LessEqual(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class GreatEqual : public BinExp {
	public:
		GreatEqual(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Add : public BinExp {
	public:
		Add(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		//virtual void assem(string * exp_str); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Subtract : public BinExp {
	public:
		Subtract(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		//virtual void assem(string * exp_str); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Divide : public BinExp {
	public:
		Divide(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Multiply : public BinExp {
	public:
		Multiply(Exp * e1, Exp * e2) { 
			this->e1 = e1; this->e2 = e2; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		//virtual void assem(string * exp_str); 
		virtual void assem(string * exp_str, string * branchLabel); 
};    // End Binary Op

	    // Unary Ops
class UnaryExp : public Exp { 
	public:
		Exp * e = NULL;
		void setTable(TableNode * t);
};
class Not : public UnaryExp {
	public:
		Not(Exp * e) { 
			this->e = e; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Pos : public UnaryExp {
	public:
		Pos(Exp * e) { 
			this->e = e; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		//virtual void assem(string * exp_str); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class Neg : public UnaryExp {
	public: 
		Neg(Exp * e) { 
			this->e = e; 
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		//virtual void assem(string * exp_str); 
		virtual void assem(string * exp_str, string * branchLabel); 
};    // End Unary Ops

class ParenExp : public UnaryExp {
	public: // For: ( e )
		ParenExp(Exp * e) {
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		int evaluate(); 
		//virtual void assem(string * exp_str); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class ArrayAccess : public Exp {
	public:
		Ident * i = NULL;
		Index * ind = NULL;
		ArrayAccess(Ident * i, Index * ind){
			this->i = i;
			this->ind = ind;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
class Length : public Exp {
	public:
		Ident * i = NULL;
		Length(Ident * i){
			this->i = i;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
class ArrayAccessLength : public Exp {
	public:
		Ident * i = NULL;
		Index * ind = NULL;
		ArrayAccessLength(Ident * i, Index * ind){
			this->i = i;
			this->ind = ind;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};
class LitInt : public Exp {
	public:
		IntLiteral * i = NULL;
		LitInt(IntLiteral * i){
			this->i = i;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel);
		//virtual void assem(string * exp_str); 
};
class True : public Exp {
	public:
		True() { lineRecord = yylineno; }
		void traverse();
		void setTable(TableNode * t);
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class False : public Exp {
	public:
		False() { lineRecord = yylineno; }
		void traverse();
		void setTable(TableNode * t);
		int evaluate(); 
		virtual void assem(string * exp_str, string * branchLabel); 
};
class ExpObject : public Exp {
	public:
		Object * o = NULL;
		ExpObject(Object * o){
			this->o = o;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
};
class ObjectMethodCall : public Exp {
	public:
		Object * o = NULL;
		Ident * i = NULL;
		ExpList * e = NULL;
		ObjectMethodCall(Object * o, Ident * i, ExpList * e){
			this->o = o;
			lineRecord = yylineno;
			this->i = i;
			this->e = e;
		}
		void traverse();
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
};
/* End Expression Abstract Classes */

/* Begin Abstract Class Object */
class Object : public TreeNode {
	public:
		virtual void setTable(TableNode * t) = 0;
		virtual void assem(string * exp_str, string * branchLabel){
			cout << "ANNNNND this object function is not here :D" << endl;
		}
};
class IdObj : public Object {
	public: // For: id
		Ident * i = NULL;
		IdObj(Ident * i) {
			lineRecord = yylineno;
			this->i = i;
		}
		void traverse();
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
};
class ThisObj : public Object {
	public: // For: this
		ThisObj() { lineRecord = yylineno; }
		void traverse();
		void setTable(TableNode * t);
};
class NewIdObj : public Object {
	public: // For: new id ()
		Ident * i = NULL;
		NewIdObj(Ident * i) {
			this->i = i;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		void evaluate();
};
class NewTypeObj : public Object {
	public:
		PrimeType * p = NULL;
		Index * i = NULL;
		NewTypeObj(PrimeType * p, Index * i) {
			this->p = p;
			this->i = i;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
};
/* End Abstract Class Object */

/* list of ",exp" ",exp"..... */
class ExpRestList : public TreeNode {
	public:
		vector<ExpRest * > * erVector = NULL;
		ExpRestList(ExpRest * e) {
			erVector = new vector<ExpRest * >;
			erVector->push_back(e);
			lineRecord = yylineno;
		}
		void append(ExpRest * e) {
			erVector->push_back(e);
		}
		void traverse();
		void setTable(TableNode * t);
};

/* ExpRestList can be NULL */
class ExpList : public TreeNode { 
	public:
		Exp * e = NULL;
		ExpRestList * erl = NULL;
		ExpList(Exp * e, ExpRestList * erl) {
			this->e = e;
			this->erl = erl;
			lineRecord = yylineno;
		} 
		void traverse();
		void setTable(TableNode * t);
};

class ExpRest : public TreeNode {
	public:
		Exp * e = NULL;
		ExpRest(Exp * e) {
			this->e = e;
			lineRecord = yylineno;
		}
		void traverse();
		void setTable(TableNode * t);
};

class Ident : public TreeNode {
	public:
		string * id = NULL;
		Ident(string * id){
			this->id = id;
			lineRecord = yylineno;
		}
		void traverse();
};

class StringLiteral : public TreeNode {
	public:
		string * str = NULL;
		string * label = NULL;
		StringLiteral(string * str){
			this->str = str;
			lineRecord = yylineno;
		}
		void traverse();
		void assem();
};

class IntLiteral : public TreeNode {
	public:
		int i = 0;
		IntLiteral(int i){
			this->i = i;
			lineRecord = yylineno;
		}
		void traverse();
};

class BoolLiteral : public TreeNode {
	public:
		bool val;
		BoolLiteral(bool val){
			this->val = val;
			lineRecord = yylineno;
		}
		void traverse() { 
			fprintf(stderr, "\nHuzzah! Your code shouln't be calling me! :/\n"); 
		};
};
