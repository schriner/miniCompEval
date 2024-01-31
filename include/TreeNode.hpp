
#ifndef TREENODE_H
#define TREENODE_H

/*
 *
 * miniCompEval 
 * Sonya Schriner
 * File: TreeNode.hpp
 *
 */

#include <vector>
#include <iostream>

#include "TableNode.hpp"

#ifdef PRINTTREE
#define PRINTDEBUGTREE(x) fprintf(stderr, "At Node: %s\n", (x));
#else
#define PRINTDEBUGTREE(x)
#endif

#ifdef ASSEM
#define PRINTLN_EXP "pln"
#define PRINT_EXP "p"
#endif

using namespace std;

extern int yylineno;
extern char * exe;
extern char * yyfilename;
extern bool programTypeError;

// Forward Class Declarations 
class MainClass;
class ClassDeclList;
class ClassDecl;
class VarDeclList;
class VarDecl; 
class MethodDeclList;
class MethodDecl;  
class FormalList;
class FormalRestList;
class FormalRest;
class PrimeType;
class Type;
class StatementList;
class Statement;
class Index;
class Exp;
class Object;
class ExpList;
class Ident;
class IntLiteral;
class StringLiteral;
struct _SYM;

typedef union { 
	int exp; // bool or int
	int * exp_single; // bool or int single indx array
	string * name;	// classname
	map<string, _SYM> * id; // identifier type
} VAL; 
typedef struct _SYM {
	VAL val;
	Type * t;
} SYM;

// Parent Abstract Class of Everything
class TreeNode {
	protected:
		TreeNode() : lineRecord(yylineno) {}
	public: 
		TableNode * lowestT = nullptr;
		Data * data = nullptr; 
		int lineRecord = -1;
		virtual void traverse() = 0; 
		void reportLine() {
			fprintf(stderr, "%s: Report Line: %d\n", exe, lineRecord);
			FILE * f = fopen(yyfilename, "r");
			char buf[256] = "";
			for (auto i = 0; i < lineRecord; i++) {
				fgets(buf, 256, f);
			}
			buf[255] = '\0';
			fprintf(stderr, "%s: %s", exe, buf);
			fclose(f);
		}
		string error_msg = "Type Violation in Line";
		void reportError() {
			programTypeError = true;
			fprintf(stderr, "%s: %s , lineno:%d\n", exe, error_msg.c_str(), lineRecord);
			fprintf(stderr, "%s: %s:%d\n", exe, yyfilename, lineRecord);
			
			FILE * f = fopen(yyfilename, "r");
			char buf[256] = "";
			for (auto i = 0; i < lineRecord; i++) {
				fgets(buf, 256, f);
			}
			buf[255] = '\0';
			fprintf(stderr, "%s: %s", exe, buf);
			fclose(f);
			abort();
		}
};

class Program : public TreeNode {
	public:
		MainClass * m;
		ClassDeclList * c;

		// (TYPECHECK:ss)
		// Use these during AST generation for EXPR
		map<string, ClassDecl *> class_table;
		vector<map<string, SYM> *>call_stack; // Fixme(ss) call stack - object with variables
		vector<map<string, SYM> *> scope_stack; // something like a sym table
		ExpList * arg_stack = nullptr;
#ifdef ASSEM
		vector<string * > * dataSection;
		vector<string * > * textSection;
		Program (MainClass * m)
			: m(m), c(nullptr), dataSection(nullptr), textSection(nullptr) {}
		Program (MainClass * m, ClassDeclList * c)
			: m(m), c(c), dataSection(nullptr), textSection(nullptr) {}
		void assem();
#else
		Program (MainClass * m, ClassDeclList * c)
			: m(m), c(c) {}
		Program (MainClass * m)
			: m(m), c(nullptr) {}
		VAL return_reg;
		void evaluate();
#endif
		void traverse(); 
};

class MainClass : public TreeNode {
	public:
		Ident * i1;
		Ident * i2;
		Statement * s;
		vector<string * > * instr = nullptr;
		MainClass (Ident * i1, Ident * i2, Statement * s)
			: i1(i1), i2(i2), s(s) {}
		void traverse();
#ifdef ASSEM
		void assem();
#else
		void evaluate();
#endif
};

class ClassDeclList : public TreeNode {
	public:
		vector<ClassDecl * > * cdVector = nullptr;
		vector<string * > * instr = nullptr;
		ClassDeclList(ClassDecl * c) { 
			cdVector = new vector<ClassDecl * >;
			cdVector->push_back(c);
			instr = new vector<string * >;
		}
		// Add an element to the 
		void append(ClassDecl * c) {
			cdVector->push_back(c);
		}
		void traverse();
#ifdef ASSEM
		void assem();
#else
		void evaluate();
#endif
};

/* Abstract class ClassDecl Start */
class ClassDecl : public TreeNode {
	public:
		VarDeclList * v = nullptr;
		MethodDeclList * m = nullptr;
		ClassDecl (VarDeclList * v,  MethodDeclList * m) : v(v), m(m) {}
#ifdef ASSEM
		virtual void assem() = 0;
#else 
		map<string, MethodDecl *> method_table;
		map<string, SYM> var_table;
		virtual void evaluate () {
			cerr << "Class Decl Evaluate Unimplemented\n";
		}
#endif
};
class ClassDeclSimple : public ClassDecl {
	public:
		Ident * i = nullptr;

		ClassDeclSimple (Ident * i, VarDeclList * v,  MethodDeclList * m)
			: ClassDecl(v, m), i(i) { 
#ifdef ASSEM
			nameTable = new map<string, string*>;
			typeTable = new map<string, string*>;
#endif
		}
		void traverse();
#ifdef ASSEM
		map<string, string*> * nameTable = nullptr; // id -> symRegLabel
		map<string, string*> * typeTable = nullptr; // id -> symRegLabel
		virtual void assem();
#else
		void evaluate();
#endif
};
class ClassDeclExtends : public ClassDecl {
	public:
		Ident * i1 = nullptr; Ident * i2 = nullptr;
		ClassDeclExtends (Ident * i1, Ident * i2, VarDeclList * v,  MethodDeclList * m) 
			: ClassDecl(v, m), i1(i1), i2(i2) {}
		void traverse();
#ifdef ASSEM
		virtual void assem();
#else
		 //void evaluate();
#endif
};
/* Abstract class ClassDecl End */

class VarDecl : public TreeNode {
	public: 
		Type * t = nullptr;
		Ident * i = nullptr;
		string * label = nullptr;
		VarDecl(Type * t, Ident * i) : t(t), i(i) {} 
		void traverse();
		// TODO(ss): void evaluate();
#ifdef ASSEM
		void setTable(TableNode * ta);
		void assem(map<string, string*> *, map<string, string*> *);
#else
		void evaluate();
#endif
};

class VarDeclList : public TreeNode { 
	public:
		vector<VarDecl * > * vdVector = nullptr;
		VarDeclList(VarDecl * v) {
			vdVector = new vector<VarDecl * >;
			vdVector->push_back(v);
		}
		void append(VarDecl * v) {
			vdVector->push_back(v);
		}
		void traverse();
#ifdef ASSEM
		void setTableNodes(TableNode * n) {
			lowestT = n;
			for (auto v = vdVector->begin(); v < vdVector->end(); v++) {
				(*v)->setTable(n);
			}
		}
		void assem(map<string, string*> *, map<string, string*> *);
#else
		void evaluate();
#endif
};

class MethodDecl : public TreeNode {
	public:
		Type * t = nullptr;
		Ident * i = nullptr;
		FormalList * f = nullptr;
		VarDeclList * v = nullptr; 
		StatementList * s = nullptr;
		Exp * e = nullptr;
		map<string, string*> * nameTable = nullptr; // id -> symRegLabel
		map<string, string*> * typeTable = nullptr; // id -> symRegLabel

		MethodDecl(Type * t, Ident * i, FormalList * f, 
				VarDeclList * v, StatementList * s, Exp * e)
			: t(t), i(i), f(f), v(v), s(s), e(e) {
			nameTable = new map<string, string*>;
			typeTable = new map<string, string*>;
		} 
		void traverse();
#ifdef ASSEM
		void assem(string * objName);
#else
		VAL evaluate();
#endif
};

class MethodDeclList : public TreeNode {
	public:
		vector<MethodDecl * > * mdVector = nullptr;
		MethodDeclList (MethodDecl * m) { 
			mdVector = new vector<MethodDecl * >;
			mdVector->push_back(m);
		}
		void append(MethodDecl * m) {
			mdVector->push_back(m);
		}
		void traverse();
#ifdef ASSEM
		void setTableParentNodes(TableNode * p);
		virtual void assem(string * objName);
#else
		void evaluate();
#endif
};

class FormalRest : public TreeNode {
	public:
		Type * t = nullptr;
		Ident * i = nullptr;
		FormalRest(Type * t, Ident * i) : t(t), i(i) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};

class FormalList : public TreeNode {
	public:
		Type * t = nullptr;
		Ident * i = nullptr;
		vector<FormalRest * > * frVector = nullptr;
		FormalList(Type * t, Ident * i, vector<FormalRest * > * frVector)
			: frVector(frVector) { (*frVector)[0] = new FormalRest(t, i); }
		FormalList(Type * t, Ident * i)	: t(t), i(i) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};

/* Abstract Class Type */
class Type : public TreeNode {
	public:
#ifdef ASSEM
		virtual void setTable(TableNode * t) = 0;
#endif
};
/* Begin Abstract Class PrimeType */
class PrimeType : public Type {
	public:
#ifdef ASSEM
		virtual void setTable(TableNode * t) = 0;
#endif
};  
class IntType : public PrimeType { 
	public: 
		IntType() {} 
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
class BoolType : public PrimeType { 
	public: 
		BoolType() {} 
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
class IdentType : public PrimeType { 
	public: 
		Ident * i = nullptr;
		IdentType(Ident * i) : i(i) {} 
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
/* End Abstract Class PrimeType */

class TypeIndexList : public Type {
	public:
		Type * t = nullptr;
		TypeIndexList(Type * t) : t(t) {}
		TypeIndexList * getLastNull() { // for the type_bracket_list non-term
			TypeIndexList * node = this;
			// Find the nullptr statement
			while (node->t != nullptr){
				node = (TypeIndexList *) node->t;
			}
			return node;
		}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
/* End Class Type */

/* Abstract Class Statement Start */
class Statement : public TreeNode { 
	public:
#ifdef ASSEM
		virtual void setTable(TableNode * t) = 0;
		virtual void assem(string * stmt_str, map<string, string*> *) {
			cerr << "\nUnimplemented Statement in assemble" << endl;
		}
#else
		virtual void evaluate() {
			cerr << "\nUnimplemented Statement in interpreter" << endl;
		}
#endif
};
class BlockStatements : public Statement {
	// For: { StatementList }  -- SL = nullptr for: { } 
	public:
		StatementList * s = nullptr;
		BlockStatements(StatementList * s) : s(s) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class IfStatement : public Statement {
	// For: if (e) s_if else s_el 
	public:
		Exp * e = nullptr;
		Statement * s_if = nullptr; Statement * s_el = nullptr;
		IfStatement( Exp * e, Statement * s_if, Statement * s_el ) 
			: e(e), s_if(s_if), s_el(s_el) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class WhileStatement : public Statement {
	// For: while (e) s
	public:
		Exp * e = nullptr;
		Statement * s = nullptr;
		WhileStatement(Exp * e, Statement * s) : e(e), s(s) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class PrintLineExp : public Statement {
	// For: .println( e )
	public:
		Exp * e = nullptr;
		PrintLineExp(Exp * e) : e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class PrintLineString : public Statement {
	// For: .println( s )
	public:
		StringLiteral * s = nullptr;
		PrintLineString (StringLiteral * s) : s(s) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class PrintExp : public Statement {
	// For: .print( e )
	public:
		Exp * e = nullptr;
		PrintExp (Exp * e) : e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class PrintString : public Statement {
	// For: .print( s )
	public:
		StringLiteral * s = nullptr;
		PrintString (StringLiteral * s) : s(s) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class Assign : public Statement {
	// For: i = e
	public:
		Ident * i = nullptr;
		Exp * e = nullptr;
		Assign(Ident * i, Exp * e) : i(i), e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class IndexAssign : public Statement {
	// For: i ind = e
	public:
		Ident * i = nullptr;
		Index * ind = nullptr;
		Exp * e = nullptr;
		IndexAssign(Ident * i, Index * ind, Exp * e) : i(i), ind(ind), e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
class ReturnStatement : public Statement {
	// For: return e 
	public:
		Exp * e = nullptr;
		ReturnStatement(Exp * e) : e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};
/* Abstract Class Statement End */

class StatementList : public TreeNode {
	public:
		vector<Statement * > * sVector = nullptr;
		StatementList(Statement * s) {
			sVector = new vector<Statement * >;
			sVector->push_back(s);
		}
		void append(Statement * s) {
			sVector->push_back(s);
		}
		void traverse();
#ifdef ASSEM
		void setTableNodes(TableNode * n);
		void assem(string * stmt_str, map<string, string*> *);
#else
		void evaluate();
#endif
};

/* Begin Abstract Class Index */
class Index : public TreeNode {
	public:
#ifdef ASSEM
		virtual void setTable(TableNode * t) = 0;
#else
		virtual int evaluate() {
			cerr << "Index unimplemented" << endl;
			return 0;
		}
#endif
}; 
class SingleIndex : public Index {
	public:
		Exp * e = nullptr;
		SingleIndex(Exp * e) : e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#else
		int evaluate();
#endif
};
class MultipleIndices : public Index {
	public:
		vector<Index *> * ind = nullptr;
		MultipleIndices(Index * i, Exp * e) :
			ind( new vector<Index *> {i, new SingleIndex( e )} ) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
/* End Abstract Class Index */

/* Start Expression Abstract Classes */
class Exp : public TreeNode {
	public:
		virtual ~Exp() = default;
#ifdef ASSEM
		virtual void setTable(TableNode * t) {
			cerr << "Set Table Undefined" << endl;
		}
		virtual void assem(string * exp_str, string * branchLabel) {
			*exp_str = *exp_str + "\n\tError(Debug):Next Assem expr unimplemented ";
		} 
#else
		// for bool values evaluate stuff to 0 and 1
		virtual VAL evaluate() {
			cerr << endl << "\tError: Expr function not implemented" << endl;
			return {0};
		} 
#endif
}; 
			// Start Op
class UnaryExp : public Exp { 
	protected:
		UnaryExp(Exp * e) : e(e) {};
	public:
		Exp * e = nullptr;
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
class BinExp : public Exp { 
	protected:
		BinExp(Exp * e1, Exp * e2) : e1(e1), e2(e2) {}; 
	public:
		Exp * e1 = nullptr;
		Exp * e2 = nullptr; 
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
class BoolResExp : public BinExp {
	public:
		BoolResExp(Exp * e1, Exp * e2)
			: BinExp(e1, e2) {}
}; 
class IntResExp : public BinExp {
	public:
		IntResExp(Exp * e1, Exp * e2)
			: BinExp(e1, e2) {
				// (TYPECHECK:ss)
				// if not integer literal or intres expr or id with type int type error
				// if ObjectMethodCall check e type
				// if ExpObject fixme 
				// check they type of the expr within the parentheses
			}
}; 
	    // Start Binary op
class Or : public BoolResExp { 
	public:
		Or(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {} 
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel);
#else
		VAL evaluate(); 
#endif
};
class And : public BoolResExp {
	public:
		And(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {} 
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel);
#else
		VAL evaluate(); 
#endif
};
class Equal : public BoolResExp {
	public:
		Equal(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {} 
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class NotEqual : public BoolResExp {
	public:
		NotEqual(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {}
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Lesser : public BoolResExp {
	public:
		Lesser(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {} 
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Greater : public BoolResExp {
	public:
		Greater(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {}
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class LessEqual : public BoolResExp {
	public:
		LessEqual(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {} 
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class GreatEqual : public BoolResExp {
	public:
		GreatEqual(Exp * e1, Exp * e2) : BoolResExp(e1, e2) {}
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Add : public IntResExp {
	public:
		Add(Exp * e1, Exp * e2) : IntResExp(e1, e2) {
		} 
		void traverse();
#ifdef ASSEM
	  void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Subtract : public IntResExp {
	public:
		Subtract(Exp * e1, Exp * e2) : IntResExp(e1, e2) {}
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Divide : public IntResExp {
	public:
		Divide(Exp * e1, Exp * e2) : IntResExp(e1, e2) {}
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Multiply : public IntResExp {
	public:
		Multiply(Exp * e1, Exp * e2) : IntResExp(e1, e2) {}
		void traverse();
#ifdef ASSEM
		void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};    // End Binary Op

	    // Unary Ops
class Not : public UnaryExp {
	public:
		Not(Exp * e) : UnaryExp(e) {
			// (TYPECHECK:ss)
			// if not boolean literal or bool res expr or id with type bool return 
			// also check the contents of a paren expr
		}
		void traverse();
#ifdef ASSEM
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Pos : public UnaryExp {
	public:
		Pos(Exp * e) : UnaryExp(e) {
				// (TYPECHECK:ss)
				// if not integer literal or intres expr or id with type int type error
				// check the contents of a paren expr 
		}
		void traverse();
#ifdef ASSEM
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class Neg : public UnaryExp {
	public: 
		Neg(Exp * e) : UnaryExp(e) {
				// (TYPECHECK:ss)
				// if not integer literal or intres expr or id with type int type error
				// check the contents of a paren expr 
		}
		void traverse();
#ifdef ASSEM
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};    // End Unary Ops

class ParenExp : public UnaryExp {
	public: // For: ( e )
		ParenExp(Exp * e) : UnaryExp(e) {}
		void traverse();
#ifdef ASSEM
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class ArrayAccess : public Exp {
	public:
		Ident * i = nullptr;
		Index * ind = nullptr;
		ArrayAccess(Ident * i, Index * ind) : i(i), ind(ind){
			// (TYPECHECK:ss)
			// if not integer literal or intres expr or id with type int type error
			// also check the contents of a paren expr
		}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#else
		VAL evaluate();
#endif
};
class Length : public Exp {
	public:
		Ident * i = nullptr;
		Length(Ident * i) : i(i) {
			// (TYPECHECK:ss)
			// Check if the id is an array expr
			// also check the contents of a paren expr
		}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#else
		VAL evaluate();
#endif
};
class ArrayAccessLength : public Exp {
	public:
		Ident * i = nullptr;
		Index * ind = nullptr;
		ArrayAccessLength(Ident * i, Index * ind) : i(i), ind(ind) {
			// (TYPECHECK:ss)
			// Check if the id is an array expr
			// also check the contents of a paren expr
		}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#else
		VAL evaluate();
#endif
};
class LitInt : public Exp {
	public:
		IntLiteral * i = nullptr;
		LitInt(IntLiteral * i) : i(i) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel);
#else
		VAL evaluate(); 
#endif
};
class True : public Exp {
	public:
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class False : public Exp {
	public:
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate(); 
#endif
};
class ExpObject : public Exp {
	public:
		Object * o = nullptr;
		ExpObject(Object * o) : o(o) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate();
#endif
};
class ObjectMethodCall : public Exp {
	public:
		Object * o = nullptr;
		Ident * i = nullptr;
		ExpList * e = nullptr;
		ObjectMethodCall(Object * o, Ident * i, ExpList * e) : o(o), i(i), e(e) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		VAL evaluate();
#endif
};
/* End Expression Abstract Classes */

/* Begin Abstract Class Object */
class Object : public TreeNode {
	public:
#ifdef ASSEM
		virtual void setTable(TableNode * t) = 0;
		virtual void assem(string * exp_str, string * branchLabel){
			cerr << "Object function unimplemented" << endl;
		}
#endif
};
class IdObj : public Object {
	public: // For: id
		Ident * i = nullptr;
		IdObj(Ident * i) : i(i) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		//TODO: void evaluate();
#endif
};
class ThisObj : public Object {
	public: // For: this
		ThisObj() {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
};
class NewIdObj : public Object {
	public: // For: new id ()
		Ident * i = nullptr;
		NewIdObj(Ident * i) : i(i) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#else
		void evaluate();
#endif
};
class NewTypeObj : public Object {
	public:
		PrimeType * p = nullptr;
		Index * i = nullptr;
		NewTypeObj(PrimeType * p, Index * i) : p(p), i(i) {}
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
		virtual void assem(string * exp_str, string * branchLabel); 
#else
		void evaluate();
#endif
};
/* End Abstract Class Object */

/* ExpRestList can be nullptr */
/* erl: list of ",exp" ",exp"..... */
class ExpList : public TreeNode { 
	public:
		// Union and dynamic cast to eliminate extra pointer
		Exp * e = nullptr;
		vector<Exp * > * erlVector = nullptr;
		ExpList(Exp * e, vector<Exp * > * erl) : erlVector(erl) {
			(*erlVector)[0] = e;
		}
		ExpList(Exp * e) : e(e) {} 
		void traverse();
#ifdef ASSEM
		void setTable(TableNode * t);
#endif
		// TODO: void evaluate();
		~ExpList() {
			if (erlVector) {
				for (auto exp = erlVector->begin(); exp < erlVector->end(); exp++) {
					delete *exp;
				}
				delete erlVector;
			}
			if (e) {
				delete e;
			}
		}
};

class Ident : public TreeNode {
	public:
		string * id = nullptr;
		Ident(string * id) : id(id) {}
		void traverse();
};

class StringLiteral : public TreeNode {
	public:
		string * str = nullptr;
		string * label = nullptr;
		StringLiteral(string * str) : str(str) {}
		void traverse();
#ifdef ASSEM
		void assem();
#endif
};

class IntLiteral : public TreeNode {
	public:
		int i = 0;
		IntLiteral(int i) : i(i) {}
		void traverse();
};

class BoolLiteral : public TreeNode {
	public:
		bool val;
		BoolLiteral(bool val) : val(val) {}
		void traverse();
};

#endif
