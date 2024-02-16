
/*
 *
 * miniCompEval
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
	VarDeclExp * vde;
	VarDeclExpList * vdel;
	MethodDeclList * mdl;
	MethodDecl * md;
	FormalList * fl;
	vector<FormalRest * > * frl;
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
	vector<Exp * > * erl;
	Exp * er;
	IntLiteral * in_l;
	//BoolLiteral * b_l;
	StringLiteral * st_l;
	Ident * id;
}

%{

#include <unistd.h>
#include <stdio.h>

int yylex();
void yyerror(const char *s);	
void yyrestart(FILE * fp);

#ifdef DEBUG_BISON
int yydebug = 1;
#endif

// Root of AST
extern Program * programRoot; 
#ifdef ASSEM
#endif
extern bool programTypeError;
string * typeStringHolder = nullptr;

%}

/* Need to use %type to declare different terminals as different things */
%type<node>  program 
%type<main>  main_class
%type<cdl>   class_decl_list
%type<cd>    class_decl
%type<vdl>   var_decl_list
%type<vd>    var_decl
%type<vde>   var_decl_exp
%type<vdel>  var_decl_exp_list
%type<mdl>   method_decl_list
%type<md>    method_decl
%type<fl>    formal_list
%type<frl>   formal_rest_list
%type<fr>    formal_rest
%type<prt>   prime_type
%type<type>  type
%type<tp_i>  type_bracket_list
%type<sl>    statement_list
%type<s>     statement
%type<i>     index
%type<e>     full_exp or_term and_term eq_term gr_le_term add_term mul_term un_op exp
%type<obj>   object
%type<el>    exp_list
%type<erl>   exp_rest_list
%type<er>    exp_rest

%token<in_l> INTEGER_LITERAL
%token<st_l> STRING_LITERAL
%token<id>   ID

%token THIS NEW EXTENDS RETURN LENGTH
%token PRINT_STATE PRINT_STATE_LN IF ELSE FOR WHILE STRING
%token CLASS MAIN STATIC VOID PUBLIC
%token SEMI COMMA NOT PLUS MINUS EQUAL TRUE FALSE DOT
%token O_PAREN C_PAREN O_BR C_BR O_SQ C_SQ 
%token INT BOOL 
%token AND OR G_EQ L_EQ EQEQ N_EQ GREAT LESS MUL DIV

%%

program : 
 main_class class_decl_list {
 		delete programRoot;
		programRoot = new Program($1, $2);
	}
	| main_class { 
 		delete programRoot;
		programRoot = new Program($1, nullptr); 
	}
	;

main_class : 
	CLASS ID O_BR PUBLIC STATIC VOID MAIN O_PAREN STRING O_SQ C_SQ ID C_PAREN O_BR statement C_BR C_BR { 
		// Dummy node to construct table for type checking
		programRoot = new Program(nullptr, nullptr);
		$$ = new MainClass($2, $12, $15); 
	}
	;

class_decl_list :
	/* Eliminate class_decl*  */
	class_decl_list class_decl {
		$1->append( $2 );
	}
	| class_decl {
		$$ = new ClassDeclList( $1 );
	}
	;

class_decl : 
		CLASS ID O_BR var_decl_list method_decl_list C_BR
		/* with method_decl_list */ { 
		$$ = new ClassDeclSimple($2, $4, $5); 
	}
  | CLASS ID EXTENDS ID O_BR var_decl_list method_decl_list C_BR
		/* with method_decl_list */ { 
		$$ = new ClassDeclExtends($2, $4, $6, $7); 
	}
	| CLASS ID O_BR var_decl_list C_BR { 
		$$ = new ClassDeclSimple($2, $4, nullptr); 
	}
  | CLASS ID EXTENDS ID O_BR var_decl_list C_BR { 
		$$ = new ClassDeclExtends($2, $4, $6, nullptr); 
	}
	/* No var_decl */
	| CLASS ID O_BR method_decl_list C_BR 
		/* with method_decl_list */ { 
		$$ = new ClassDeclSimple($2, nullptr, $4); 
	}
  | CLASS ID EXTENDS ID O_BR method_decl_list C_BR 
		/* with method_decl_list */ { 
		$$ = new ClassDeclExtends($2, $4, nullptr, $6); 
	}
	| CLASS ID O_BR C_BR { 
		$$ = new ClassDeclSimple($2, nullptr, nullptr); 
	}
  | CLASS ID EXTENDS ID O_BR C_BR { 
		$$ = new ClassDeclExtends($2, $4, nullptr, nullptr); 
	};

var_decl_list :
	/* Eliminate var_decl* */
	var_decl_list var_decl {
		$1->append( $2 );
	}
	| var_decl {
		$$ = new VarDeclList( $1 );
	}
	;

var_decl : 
	INT ID SEMI { 
		$$ = new VarDecl( new IntType(), $2 ); 
	}
	| BOOL ID SEMI { 
		$$ = new VarDecl( new BoolType(), $2 ); 
	}
	| ID ID SEMI { 
		$$ = new VarDecl( new IdentType($1), $2 ); 
	}
	| INT type_bracket_list ID SEMI {
		$2->getLastNull()->t = new IntType();
		$$ = new VarDecl( $2, $3 ); 
	}
	| BOOL type_bracket_list ID SEMI { 
		$2->getLastNull()->t = new BoolType();
		$$ = new VarDecl( $2, $3 ); 
	}
	| ID type_bracket_list ID SEMI { 
		$2->getLastNull()->t = new IdentType($1);
		$$ = new VarDecl( $2, $3 ); 
	}
	;

var_decl_exp_list : /* Eliminate statement* */
	var_decl_exp_list var_decl_exp SEMI {
		$1->append( $2 );
	}
	| var_decl_exp_list var_decl {
		$1->append( $2 );
	}
	| var_decl_exp SEMI {
		$$ = new VarDeclExpList( $1 );
	}
	| var_decl {
		$$ = new VarDeclExpList( $1 );
	}
	;

var_decl_exp : 
	INT ID EQUAL full_exp { 
		$$ = new VarDeclExp( new IntType(), $2, new Assign($2, $4)); 
	}
	| BOOL ID EQUAL full_exp { 
		$$ = new VarDeclExp( new BoolType(), $2, new Assign($2, $4) ); 
	}
	| ID ID EQUAL full_exp { 
		$$ = new VarDeclExp( new IdentType($1), $2, new Assign($2, $4) ); 
	} 
	| INT type_bracket_list ID EQUAL NEW prime_type index {
		$2->getLastNull()->t = new IntType();
		$$ = new VarDeclExp( $2, $3, new Assign( $3, new ExpObject( new NewTypeObj( $6, $7 )))); 
	}
	| BOOL type_bracket_list ID EQUAL NEW prime_type index { 
		$2->getLastNull()->t = new BoolType();
		$$ = new VarDeclExp( $2, $3, new Assign( $3, new ExpObject( new NewTypeObj( $6, $7 ))) ); 
	}
	| ID type_bracket_list ID EQUAL NEW prime_type index { 
		$2->getLastNull()->t = new IdentType($1);
		$$ = new VarDeclExp( $2, $3, new Assign( $3, new ExpObject( new NewTypeObj( $6, $7 ))) ); 
	}
	;

type_bracket_list :
	type_bracket_list O_SQ C_SQ { 
		$$ = new TypeIndexList( $1 );
	}
	| O_SQ C_SQ {
		$$ = new TypeIndexList( nullptr );
	}
	;

method_decl_list : /* Eliminate method_decl* */
	method_decl_list method_decl { 
		$$ = $1; 
		$1->append( $2 ); 
	}
	| method_decl {
		$$ = new MethodDeclList( $1 ); 
	}
	;

method_decl : 
	PUBLIC type ID O_PAREN formal_list C_PAREN O_BR var_decl_list statement_list RETURN full_exp SEMI C_BR 
	{ 
		$$ = new MethodDecl($2, $3, $5, $8, $9, $11);  
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR var_decl_list statement_list RETURN full_exp SEMI C_BR 
	{
		$$ = new MethodDecl($2, $3, nullptr, $7, $8, $10); 
	}
	| PUBLIC type ID O_PAREN formal_list C_PAREN O_BR statement_list RETURN full_exp SEMI C_BR /* no var_decl */
	{ 
		$$ = new MethodDecl($2, $3, $5, nullptr, $8, $10); 
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR statement_list RETURN full_exp SEMI C_BR /* no var_decl */
	{ 
		$$ = new MethodDecl($2, $3, nullptr, nullptr, $7, $9); 
	}
	/* No Statement List */
	| PUBLIC type ID O_PAREN formal_list C_PAREN O_BR var_decl_list RETURN full_exp SEMI C_BR
	{
		$$ = new MethodDecl($2, $3, $5, $8, nullptr, $10); 
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR var_decl_list RETURN full_exp SEMI C_BR
	{ 
		$$ = new MethodDecl($2, $3, nullptr, $7, nullptr, $9); 
	}
	| PUBLIC type ID O_PAREN formal_list C_PAREN O_BR  RETURN full_exp SEMI C_BR
		/* no var_decl */
	{
		$$ = new MethodDecl($2, $3, $5, nullptr, nullptr, $9); 
	}
	| PUBLIC type ID O_PAREN C_PAREN O_BR  RETURN full_exp SEMI C_BR
		/* no var_decl */
	{
		$$ = new MethodDecl($2, $3, nullptr, nullptr, nullptr, $8); 
	}
	;

formal_list : 
	type ID formal_rest_list { 
		$$ = new FormalList( $1, $2, $3 ); 
	}
	| type ID { 
		$$ = new FormalList( $1, $2 ); 
	}
	;

formal_rest_list : /* Eliminate formal_rest* */
	formal_rest_list formal_rest {
		$1->push_back( $2 );
	}
	| formal_rest /* empty rule */ {
		$$ = new vector<FormalRest * >{nullptr, $1};
	}
	;

formal_rest :
	COMMA type ID { 
		$$ = new FormalRest( $2, $3 ); 
	}
	;

type : 
	prime_type {
		$$ = dynamic_cast<Type *> ($1) ;
	}
	| type O_SQ C_SQ {
		$$ = new TypeIndexList( $1 );
	}
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
	statement_list statement {
		if ($2) $1->append( $2 );
	}
	| statement {
		$$ = new StatementList( $1 );
	}
	;

statement : 
	  O_BR C_BR /* No statement */ {
		$$ = new BlockStatements(nullptr);
	}
	| O_BR statement_list C_BR { 
		$$ = new BlockStatements($2);
	} 
	| O_BR var_decl_exp_list C_BR { 
		// Do nothing if there are variable declarations but no statements 
		$$ = new BlockStatements(nullptr);
	} 
	| O_BR var_decl_exp_list statement_list C_BR { 
		$$ = new BlockStatements($2, $3);
	} 
  | IF O_PAREN full_exp C_PAREN statement ELSE statement {
		$$ = new IfStatement( $3, $5, $7 );
	}
  | WHILE O_PAREN full_exp C_PAREN statement {
		$$ = new WhileStatement( $3, $5 );
	}
  | FOR O_PAREN var_decl_exp SEMI full_exp SEMI ID EQUAL full_exp C_PAREN statement {
		$$ = new ForStatement( $3, $5, new Assign( $7, $9), $11 );
	}
  | PRINT_STATE O_PAREN full_exp C_PAREN SEMI { 
		$$ = new PrintExp( $3 );
	}
  | PRINT_STATE O_PAREN STRING_LITERAL C_PAREN SEMI { 
		$$ = new PrintString( $3 );
	}
  | PRINT_STATE_LN O_PAREN full_exp C_PAREN SEMI {
		$$ = new PrintLineExp( $3 );
	}
  | PRINT_STATE_LN O_PAREN STRING_LITERAL C_PAREN SEMI { 
		$$ = new PrintLineString( $3 );
	}
  | ID EQUAL full_exp SEMI {
		$$ = new Assign( $1, $3 );
	}
  | ID index EQUAL full_exp SEMI {
		$$ = new IndexAssign( $1, $2, $4);
	}
  | RETURN full_exp SEMI {
		$$ = new ReturnStatement( $2 );
	}
	;

index : 
	O_SQ full_exp C_SQ { 
		$$ = new SingleIndex( $2 );
	}
	| index O_SQ full_exp C_SQ {
		$$ = new MultipleIndices( $1, $3 );
	}
	; 
	
full_exp : /* Full expression */
	or_term
	;

or_term : /* or has the lowest presedence */
	and_term
	| or_term OR and_term {
		$$ = new Or( $1, $3 );
	}
	;

and_term : /* and has a higher presedence than or */
	eq_term
	| and_term AND eq_term {
		$$ = new And( $1, $3 ); 
	}
	;

eq_term : /* Equal to operations */
	gr_le_term
	| eq_term EQEQ gr_le_term {
		$$ = new Equal( $1, $3 );
	}
	| eq_term N_EQ gr_le_term {
		$$ = new NotEqual( $1, $3 );
	}
	;

gr_le_term : /* Greater than less than ops */ 
	add_term
	| gr_le_term LESS add_term { 
		$$ = new Lesser( $1, $3 ); 
	}
	| gr_le_term GREAT add_term { 
		$$ = new Greater( $1, $3 );
	}
	| gr_le_term L_EQ add_term { 
		$$ = new LessEqual( $1, $3 ); 
	}
	| gr_le_term G_EQ add_term { 
		$$ = new GreatEqual( $1, $3 );
	}
	;

add_term : /* Expression with + - op */
	mul_term 
	| add_term PLUS mul_term {
		$$ = new Add( $1, $3 );
	}
	| add_term MINUS mul_term {
		$$ = new Subtract( $1, $3 );
	}
	;

mul_term : /* Expression with multiplication or division */
	un_op
	| mul_term DIV un_op { 
		$$ = new Divide( $1, $3 );
	}
	| mul_term MUL un_op { 
		$$ = new Multiply( $1, $3 );
	}
	;

un_op : /* Expression with op */
	exp
	| NOT un_op {
		$$ = new Not( $2 );
	}
	| PLUS un_op { 
		$$ = new Pos( $2 );
	}
	| MINUS un_op { 
		$$ = new Neg( $2 ); 
	}
	;

exp : 
	  ID { 
		$$ = new ExpObject(new IdObj( $1 ));
	} 
	| ID index { 
		$$ = new ArrayAccess( $1, $2 );
	}
	| ID DOT LENGTH {
		$$ = new Length( $1 ); 
	}
	| ID index DOT LENGTH { 
		$$ = new ArrayAccessLength( $1, $2 ); 
	}
	| INTEGER_LITERAL { 
		$$ = new LitInt( $1 ); 
	}
	| TRUE {
		$$ = new True(); 
	}
	| FALSE {
		$$ = new False(); 
	}
	| object {
		$$ = new ExpObject( $1 ); 
	}
	| ID DOT ID O_PAREN exp_list C_PAREN { 
		$$ = new ObjectMethodCall( new IdObj( $1 ), $3, $5 ); 
	}
	| object DOT ID O_PAREN exp_list C_PAREN { 
		$$ = new ObjectMethodCall( $1, $3, $5 ); 
	}
	| ID DOT ID O_PAREN C_PAREN { 
		$$ = new ObjectMethodCall( new IdObj( $1 ), $3, nullptr ); 
	}
	| object DOT ID O_PAREN C_PAREN { 
		$$ = new ObjectMethodCall( $1, $3, nullptr ); 
	}
	| O_PAREN full_exp C_PAREN {
		$$ = new ParenExp( $2 ); 
	}
	;

object : 
	THIS {
		$$ = new ThisObj();
	} 
	| NEW ID O_PAREN C_PAREN {
		$$ = new NewIdObj( $2 );
	}
	| NEW prime_type index {
		$$ = new NewTypeObj( $2, $3 );
	}
	;

exp_rest_list : /* Eliminate exp_rest* */
	exp_rest_list exp_rest {
		$1->push_back( $2 );
	}
	| exp_rest {
		$$ = new vector<Exp * > {nullptr, $1 };
	}
	;

exp_list : 
	full_exp exp_rest_list {
		$$ = new ExpList( $1, $2 );
	}
	| full_exp {
		$$ = new ExpList( $1 );
	}
	;

exp_rest : 
	COMMA full_exp {
		$$ = $2 ;
	}
	;

%%

void yylineerror() {
	extern char * yyfilename;
	extern char * exe;
	FILE * f = fopen(yyfilename, "r");
	char buf[256] = "";
	for (auto i = 0; i < yylineno; i++) {
		fgets(buf, 256, f);
	}
	buf[255] = '\0';
  fprintf(stderr, "%s: %s", exe, buf);
	fclose(f);

}

/* Declare yyerror */
void yyerror (char const *s) {
	extern int yylineno;
	extern char * yytext;
	extern char * yyfilename;
	extern char * exe;
  fprintf (stderr, "%s: %s at token: %s\n%s: %s:%d\n", exe, s, yytext, exe, yyfilename, yylineno);
	yylineerror();
}
