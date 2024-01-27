
/*
 *
 * CS352: Project 3
 * Sonya Schriner
 * File: parser.y
 *
 */

%code requires
{
#include <string>
#include "TreeNode.hpp"

using namespace std;

}

%union
{
	TreeNode * node;
	MainClass * main;
	ClassDeclList * cdl;
	ClassDecl * cd;
	VarDeclList * vdl;
	VarDecl * vd;
	MethodDeclList * mdl;
	MethodDecl * md;
	FormalList * fl;
	FormalRestList * frl;
	FormalRest * fr;
	PrimeType * prt;
	Type * type;
	TypeIndexList * tp_i;
	StatementList * sl;
	Statement * s;
	Index * i;
	Exp * e;
	Object * obj; 
	ExpList * el;
	ExpRestList * erl;
	Exp * er;
	IntLiteral * in_l;
	BoolLiteral * b_l;
	StringLiteral * st_l;
	Ident * id;
}

%{


int yylex();
void yyerror(const char *s);	
void yyrestart(FILE * fp);
//int yydebug = 1;

// Root of AST
extern Program * programRoot; 
extern TableNode * rootScope; 
extern bool programTypeError;
string * typeStringHolder = NULL;

%}

/* Need to use %type to declare different terminals as different things */
%type<node> program 
%type<main> main_class
%type<cdl>  class_decl_list
%type<cd>   class_decl
%type<vdl>  var_decl_list
%type<vd>   var_decl
%type<mdl>  method_decl_list
%type<md>   method_decl
%type<fl>   formal_list
%type<frl>  formal_rest_list
%type<fr>   formal_rest
%type<prt>  prime_type
%type<type> type
%type<tp_i> type_bracket_list
%type<sl>   statement_list
%type<s>    statement
%type<i>    index
%type<e>    full_exp or_term and_term eq_term gr_le_term add_term mul_term un_op exp
%type<obj>  object
%type<el>   exp_list
%type<erl>  exp_rest_list
%type<er>   exp_rest
%type<in_l> INTEGER_LITERAL
%type<st_l> STRING_LITERAL
%type<id>   ID

%token THIS NEW EXTENDS RETURN LENGTH
%token PRINT_STATE PRINT_STATE_LN IF ELSE WHILE STRING
%token CLASS MAIN STATIC VOID PUBLIC
%token SEMI COMMA NOT PLUS MINUS EQUAL TRUE FALSE DOT
%token O_PAREN C_PAREN O_BR C_BR O_SQ C_SQ 
%token STRING_LITERAL INTEGER_LITERAL ID INT BOOL 
%token AND OR G_EQ L_EQ EQEQ N_EQ GREAT LESS MUL DIV

%%

program  : 
	main_class class_decl_list { // TODO: add main + decl to table 
		programRoot = new Program($1, $2);
		rootScope = new TableNode(NULL);
		programRoot->lowestT = rootScope;
		$1->lowestT->parent = rootScope; // main_scope->root
		// for all c class_decl: c->root
		for (auto c = $2->cdVector->begin(); c < $2->cdVector->begin(); c++) {
			(*c)->lowestT->parent = rootScope;
		}
		$2->lowestT = rootScope; // class_decl scope is root scope
		//programRoot->traverse();
		//if (!programTypeError) { programRoot->evaluate(); }
	}
	| main_class { 
		programRoot = new Program($1, NULL); 
		rootScope = new TableNode(NULL);
		programRoot->lowestT = rootScope;
		$1->lowestT->parent = rootScope; // main_scope->root
		//programRoot->traverse();
		//if (!programTypeError) { programRoot->evaluate(); }
	}
	;

main_class : 
	CLASS ID O_BR PUBLIC STATIC VOID MAIN O_PAREN STRING O_SQ C_SQ ID C_PAREN O_BR statement C_BR C_BR { 
		$$ = new MainClass($2, $12, $15); 
		$$->lowestT = new TableNode(NULL);
		$2->lowestT = $$->lowestT;
		$12->lowestT = $$->lowestT;
		$15->setTable($$->lowestT);
		// TODO: Add the contents of statement $15 to main scope
	}
	;

class_decl_list : /* Eliminate class_decl*  */
	class_decl_list class_decl { $1->append( $2 ); }
	| class_decl { $$ = new ClassDeclList( $1 ); }
	;

class_decl : 
	CLASS ID O_BR var_decl_list method_decl_list C_BR /* with method_decl_list */ { 
		$$ = new ClassDeclSimple($2, $4, $5); // TODO: Add to class_decl table
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$5->setTableParentNodes($$->lowestT); // set parent table node
		$4->setTableNodes($$->lowestT); // set table node for var_decl
		$4->lowestT = $$->lowestT; // set table node for var_decl_list
		$5->lowestT = $$->lowestT; // set table node for method_decl_list
	}
  | CLASS ID EXTENDS ID O_BR var_decl_list method_decl_list C_BR /* with method_decl_list */ { 
		$$ = new ClassDeclExtends($2, $4, $6, $7); 
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$4->lowestT = $$->lowestT;
		$7->setTableParentNodes($$->lowestT); // set parent table node 
		$6->setTableNodes($$->lowestT); // set table node for var_decl
		$6->lowestT = $$->lowestT;
		$7->lowestT = $$->lowestT;
	}
	| CLASS ID O_BR var_decl_list C_BR { 
		$$ = new ClassDeclSimple($2, $4, NULL); 
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$4->setTableNodes($$->lowestT); // set table node for var_decl
		$4->lowestT = $$->lowestT;
	}
  | CLASS ID EXTENDS ID O_BR var_decl_list C_BR { 
		$$ = new ClassDeclExtends($2, $4, $6, NULL); 
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$4->lowestT = $$->lowestT;
		$6->setTableNodes($$->lowestT); // set table node for var_decl
		$6->lowestT = $$->lowestT;
	}
	/* No var_decl */
	| CLASS ID O_BR method_decl_list C_BR /* with method_decl_list */ { $$ = new ClassDeclSimple($2, NULL, $4); 
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$4->setTableParentNodes($$->lowestT); 
		$4->lowestT = $$->lowestT;
	}
  | CLASS ID EXTENDS ID O_BR method_decl_list C_BR /* with method_decl_list */ { 
		$$ = new ClassDeclExtends($2, $4, NULL, $6); 
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$4->lowestT = $$->lowestT;
	  // for all m method_decl: m->classTableNode
		$6->setTableParentNodes($$->lowestT); 
		$6->lowestT = $$->lowestT;
	}
	| CLASS ID O_BR C_BR { 
		$$ = new ClassDeclSimple($2, NULL, NULL); 
		$2->lowestT = $$->lowestT;
		$$->lowestT = new TableNode(NULL); 
	}
  | CLASS ID EXTENDS ID O_BR C_BR { 
		$$ = new ClassDeclExtends($2, $4, NULL, NULL); 
		$$->lowestT = new TableNode(NULL); 
		$2->lowestT = $$->lowestT;
		$4->lowestT = $$->lowestT;
	};

var_decl_list : /* Eliminate var_decl* */
	var_decl_list var_decl { $1->append( $2 ); }
	| var_decl { $$ = new VarDeclList( $1 ); }
	;

var_decl : 
	INT ID SEMI { 
		$$ = new VarDecl( new TypePrime(new IntType()), $2 ); 
		$$->data = new SimpleVar($2->id, "INT", new IntLiteral(0));
	}
	| BOOL ID SEMI { 
		$$ = new VarDecl( new TypePrime(new BoolType()), $2 ); 
		//$$->data = new SimpleVar($2->id, "BOOL", new BoolLiteral(false));
	}
	| ID ID SEMI { 
		$$ = new VarDecl( new TypePrime(new IdentType($1)), $2 ); 
		// TODO: FIXME 
		$$->data = new RefVar($2->id, $1->id, NULL);
	}
	| INT type_bracket_list ID SEMI {
		$2->getLastNull()->t = new TypePrime(new IntType());
		$$ = new VarDecl( $2, $3 ); 
	}
	| BOOL type_bracket_list ID SEMI { 
		$2->getLastNull()->t = new TypePrime(new BoolType());
		$$ = new VarDecl( $2, $3 ); 
	}
	| ID type_bracket_list ID SEMI { 
		$2->getLastNull()->t = new TypePrime(new IdentType($1));
		$$ = new VarDecl( $2, $3 ); 
	}
	;

type_bracket_list :
	type_bracket_list O_SQ C_SQ { $$ = new TypeIndexList( $1 ); }
	| O_SQ C_SQ { $$ = new TypeIndexList( NULL ); }
	;

method_decl_list : /* Eliminate method_decl* */
	method_decl_list method_decl { 
		$$ = $1; 
		$1->append( $2 ); 
		$2->data = new MethodVar($2->i->id);
	}
	| method_decl { $$ = new MethodDeclList( $1 ); 
		$1->data = new MethodVar($1->i->id);
	}
	;

method_decl : 
	PUBLIC type ID O_PAREN formal_list C_PAREN O_BR var_decl_list statement_list RETURN full_exp SEMI C_BR 
	{ // TODO: Apend to table
		// DONE: set nodes for -, -, -, - , -, - 
		$$ = new MethodDecl($2, $3, $5, $8, $9, $11);  
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT); // type
		$3->lowestT = $$->lowestT; // id
		$5->setTable($$->lowestT); // formal list
		$8->setTableNodes($$->lowestT); // set table nodes for var_decl
		$8->lowestT = $$->lowestT; // set table node for var_decl_list
		$9->setTableNodes($$->lowestT); // set table nodes for statement
		$9->lowestT = $$->lowestT; // set table node for statement_list
		$11->setTable($$->lowestT); // ok set stuff for exp too
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR var_decl_list statement_list RETURN full_exp SEMI C_BR 
	{ $$ = new MethodDecl($2, $3, NULL, $7, $8, $10); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$7->setTableNodes($$->lowestT); // set table nodes for var_decl
		$7->lowestT = $$->lowestT; // set table node for var_decl_list
		$8->setTableNodes($$->lowestT); // set table nodes for statement
		$8->lowestT = $$->lowestT; // set table node for statement_list
		$10->setTable($$->lowestT); // ok set stuff for exp too
	}
	| PUBLIC type ID O_PAREN formal_list C_PAREN O_BR statement_list RETURN full_exp SEMI C_BR /* no var_decl */
	{ $$ = new MethodDecl($2, $3, $5, NULL, $8, $10); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$5->setTable($$->lowestT); // formal list
		$8->setTableNodes($$->lowestT); // set table nodes for statement
		$8->lowestT = $$->lowestT; // set table node for statement_list
		$10->setTable($$->lowestT); // ok set stuff for exp too
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR statement_list RETURN full_exp SEMI C_BR /* no var_decl */
	{ $$ = new MethodDecl($2, $3, NULL, NULL, $7, $9); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$7->setTableNodes($$->lowestT); // set table nodes for statement
		$7->lowestT = $$->lowestT; // set table node for statement_list
		$9->setTable($$->lowestT); // ok set stuff for exp too
	}
	/* No Statement List */
	| PUBLIC type ID O_PAREN formal_list C_PAREN O_BR var_decl_list RETURN full_exp SEMI C_BR
	{ $$ = new MethodDecl($2, $3, $5, $8, NULL, $10); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$5->setTable($$->lowestT); // formal list
		$8->setTableNodes($$->lowestT); // set table nodes for var_decl
		$8->lowestT = $$->lowestT; // set table node for var_decl_list
		$10->setTable($$->lowestT); // ok set stuff for exp too
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR var_decl_list RETURN full_exp SEMI C_BR
	{ $$ = new MethodDecl($2, $3, NULL, $7, NULL, $9); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$7->setTableNodes($$->lowestT); // set table nodes for var_decl
		$7->lowestT = $$->lowestT; // set table node for var_decl_list
		$9->setTable($$->lowestT); // ok set stuff for exp too
	}
	| PUBLIC type ID O_PAREN formal_list C_PAREN O_BR  RETURN full_exp SEMI C_BR /* no var_decl */
	{ $$ = new MethodDecl($2, $3, $5, NULL, NULL, $9); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$5->setTable($$->lowestT); // formal list
		$9->setTable($$->lowestT); // ok set stuff for exp too
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR  RETURN full_exp SEMI C_BR /* no var_decl */
	{ $$ = new MethodDecl($2, $3, NULL, NULL, NULL, $8); 
		$$->lowestT = new TableNode(NULL); 

		$2->setTable($$->lowestT);
		$3->lowestT = $$->lowestT;
		$8->setTable($$->lowestT); // ok set stuff for exp too
	}
	;

formal_list : 
	type ID formal_rest_list { 
		$$ = new FormalList( $1, $2, $3 ); 
		$$->data = new RefVar($2->id, typeStringHolder, NULL);
	}
	| type ID { 
		$$ = new FormalList( $1, $2, NULL ); 
		$$->data = new RefVar($2->id, typeStringHolder, NULL);
	}
	;

formal_rest_list : /* Eliminate formal_rest* */
	formal_rest_list formal_rest { $1->append( $2 ); }
	| formal_rest /* empty rule */ { $$ = new FormalRestList( $1 ); }
	;

formal_rest :
	COMMA type ID { 
		$$ = new FormalRest( $2, $3 ); 
		$$->data = new RefVar($3->id, typeStringHolder, NULL);
	}
	;

type : 
	prime_type { $$ = new TypePrime( $1 ); }
	| type O_SQ C_SQ { $$ = new TypeIndexList( $1 ); }
	;

prime_type :
	INT { 
		$$ = new IntType(); 
		typeStringHolder = new string("INT");
	}
	| BOOL {
		$$ = new BoolType(); 
		typeStringHolder = new string("BOOL");
	}
	| ID { 
		$$ = new IdentType( $1 ); 
		typeStringHolder = new string($1->id->c_str());
	}
	;

statement_list : /* Eliminate statement* */
	statement_list statement { $1->append( $2 ); }
	| statement { $$ = new StatementList( $1 ); }
	;

statement : 
	O_BR statement_list C_BR { 
		$$ = new BlockStatements($2); } 
	| O_BR C_BR /* No statement */ { $$ = new BlockStatements(NULL); }
  | IF O_PAREN full_exp C_PAREN statement ELSE statement {
		$$ = new IfStatement( $3, $5, $7 ); }
  | WHILE O_PAREN full_exp C_PAREN statement {
		$$ = new WhileStatement( $3, $5 ); }
  | PRINT_STATE O_PAREN full_exp C_PAREN SEMI
		{ $$ = new PrintExp( $3 ); }
  | PRINT_STATE O_PAREN STRING_LITERAL C_PAREN SEMI
		{ $$ = new PrintString( $3 ); }
  | PRINT_STATE_LN O_PAREN full_exp C_PAREN SEMI
		{ $$ = new PrintLineExp( $3 ); }
  | PRINT_STATE_LN O_PAREN STRING_LITERAL C_PAREN SEMI
		{ $$ = new PrintLineString( $3 ); }
  | ID EQUAL full_exp SEMI { $$ = new Assign( $1, $3 ); }
  | ID index EQUAL full_exp SEMI { $$ = new IndexAssign( $1, $2, $4); }
  | RETURN full_exp SEMI { $$ = new ReturnStatement( $2 ); }
	;

index : 
	O_SQ full_exp C_SQ { $$ = new SingleIndex( $2 ); }
	| index O_SQ full_exp C_SQ { $$ = new MultipleIndices( $1, $3 ); }
	; 
	
full_exp : /* Full expression */
	or_term
	;

or_term : /* or has the lowest presedence */
	and_term
	| or_term OR and_term { $$ = new Or( $1, $3 ); }
	;

and_term : /* and has a higher presedence than or */
	eq_term
	| and_term AND eq_term { $$ = new And( $1, $3 ); }
	;

eq_term : /* Equal to operations */
	gr_le_term
	| eq_term EQEQ gr_le_term { $$ = new Equal( $1, $3 ); }
	| eq_term N_EQ gr_le_term { $$ = new NotEqual( $1, $3 ); }
	;

gr_le_term : /* Greater than less than ops */ 
	add_term
	| gr_le_term LESS add_term { $$ = new Lesser( $1, $3 ); }
	| gr_le_term GREAT add_term { $$ = new Greater( $1, $3 ); }
	| gr_le_term L_EQ add_term { $$ = new LessEqual( $1, $3 ); }
	| gr_le_term G_EQ add_term { $$ = new GreatEqual( $1, $3 ); }
	;

add_term : /* Expression with + - op */
	mul_term 
	| add_term PLUS mul_term { $$ = new Add( $1, $3 ); }
	| add_term MINUS mul_term { $$ = new Subtract( $1, $3 ); }
	;

mul_term : /* Expression with multiplication or division */
	un_op
	| mul_term DIV un_op { $$ = new Divide( $1, $3 ); }
	| mul_term MUL un_op { $$ = new Multiply( $1, $3 ); }
	;

un_op : /* Expression with op */
	exp
	| NOT un_op { $$ = new Not( $2 ); }
	| PLUS un_op { $$ = new Pos( $2 ); }
	| MINUS un_op { $$ = new Neg( $2 ); }
	;

exp : 
	ID { $$ = new ExpObject(new IdObj( $1 )); } 
	| ID index { $$ = new ArrayAccess( $1, $2 ); }
	| ID DOT LENGTH { $$ = new Length( $1 ); }
	| ID index DOT LENGTH { $$ = new ArrayAccessLength( $1, $2 ); }
	| INTEGER_LITERAL { $$ = new LitInt( $1 ); }
	| TRUE { $$ = new True(); }
	| FALSE { $$ = new False(); }
	| object { $$ = new ExpObject( $1 ); }
	| ID DOT ID O_PAREN exp_list C_PAREN 
	  { $$ = new ObjectMethodCall( new IdObj( $1 ), $3, $5 ); }
	| object DOT ID O_PAREN exp_list C_PAREN 
		{ $$ = new ObjectMethodCall( $1, $3, $5 ); }
	| ID DOT ID O_PAREN C_PAREN 
		{ $$ = new ObjectMethodCall( new IdObj( $1 ), $3, NULL ); }
	| object DOT ID O_PAREN C_PAREN 
		{ $$ = new ObjectMethodCall( $1, $3, NULL ); }
	| O_PAREN full_exp C_PAREN { $$ = new ParenExp( $2 ); }
	;

object : 
	THIS { $$ = new ThisObj(); } 
	| NEW ID O_PAREN C_PAREN { $$ = new NewIdObj( $2 ); }
	| NEW prime_type index { $$ = new NewTypeObj( $2, $3 ); }
	;

exp_rest_list : /* Eliminate exp_rest* */
	exp_rest_list exp_rest { $1->append( $2 ); }
	| exp_rest { $$ = new ExpRestList( $1 ); }
	;

exp_list : 
	full_exp exp_rest_list { $$ = new ExpList( $1, $2 ); }
	| full_exp { $$ = new ExpList( $1, NULL ); }
	;

exp_rest : 
	COMMA full_exp { $$ = $2 ; }
	//COMMA full_exp { $$ = new ExpRest( $2 ); }
	;

%%

/* Declare yyerror */
void yyerror (char const *s) {
	extern int yylineno;
	extern char * yytext;
  fprintf (stderr, "%s in line %d\n", s, yylineno);
}
