
/*
 *
 * miniCompEval
 * Sonya Schriner
 * File: parser.l
 *
 */

%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>

#include "MiniJava_syntax.hpp"
#ifdef DEBUG_FLEX
#define PRINTDEBUG(x) fprintf(stderr, "\n%s\n", (x));
#else
#define PRINTDEBUG(x)
#endif

using namespace std;

%}

/* Regular Definitions */
ID [a-zA-Z][a-zA-Z0-9_]*
INT [0-9][0-9]*
STRING \"([^"\n\\]|(\\.?))*\"
L_COMMENT \/\/[^\n]*\n
B_COMMENT "/*"([^*]|\*+[^*/])*\*+"/"

%%

{L_COMMENT} { PRINTDEBUG(yytext); /* Ignore line comment */ }
{B_COMMENT} { PRINTDEBUG(yytext); /* Ignore block comment */ }
{STRING} { 
	PRINTDEBUG(yytext);
	string temp(yytext + 1, strlen(yytext) - 2);
#ifndef ASSEM
	string fin("");
	for (int i = 0; i < temp.length(); i++) {
		if (temp[i] == '\\' && (i + 1 < temp.length()) ) {
			i++;
			if (temp[i] == 'n') {
				fin.push_back('\n');
			} else if (temp[i] == 'b') {
				fin.push_back('\b');
			} else if (temp[i] == 't') {
				fin.push_back('\t');
			} else if (temp[i] == 'r') {
				fin.push_back('\r');
			} else if (temp[i] == 'f') {
				fin.push_back('\f');
			} else if (temp[i] == '\'') {
				fin.push_back('\'');
			} else if (temp[i] == '\"') {
				fin.push_back('\"');
			} else if (temp[i] == '\\') {
				fin.push_back('\\');
			}
		} else {
			fin.push_back(temp[i]);
		}
	}
	yylval.st_l = new StringLiteral( new string(fin) ); 
#else
	yylval.st_l = new StringLiteral( new string(temp) ); 
#endif
	return STRING_LITERAL; 
}
{INT} { PRINTDEBUG(yytext); yylval.in_l = new IntLiteral( atoi(yytext) ); return INTEGER_LITERAL; }
[ \r\t\n] { /* Discard Extra Whitespce */ }
; { PRINTDEBUG(yytext); return SEMI; }
, { PRINTDEBUG(yytext); return COMMA; }
! { PRINTDEBUG(yytext); return NOT; }
\+ { PRINTDEBUG(yytext); return PLUS; }
"-" { PRINTDEBUG(yytext); return MINUS; }
"&&" { PRINTDEBUG(yytext); return AND; }
"\|\|" { PRINTDEBUG(yytext); return OR; }
"<=" { PRINTDEBUG(yytext); return L_EQ; }
">=" { PRINTDEBUG(yytext); return G_EQ; }
"==" { PRINTDEBUG(yytext); return EQEQ; }
"!=" { PRINTDEBUG(yytext); return N_EQ; }
"=" { PRINTDEBUG(yytext); return EQUAL; }
">" { PRINTDEBUG(yytext); return GREAT; }
"<" { PRINTDEBUG(yytext); return LESS; }
\* { PRINTDEBUG(yytext); return MUL; }
\/ { PRINTDEBUG(yytext); return DIV; }
true { PRINTDEBUG(yytext); return TRUE; }
false { PRINTDEBUG(yytext); return FALSE; }
\. { PRINTDEBUG(yytext); return DOT; }
\( { PRINTDEBUG(yytext); return O_PAREN; }
\) { PRINTDEBUG(yytext); return C_PAREN; }
\{ { PRINTDEBUG(yytext); return O_BR; }
\} { PRINTDEBUG(yytext); return C_BR; }
\[ { PRINTDEBUG(yytext); return O_SQ; }
\] { PRINTDEBUG(yytext); return C_SQ; }
new { PRINTDEBUG(yytext); return NEW; }
return { PRINTDEBUG(yytext); return RETURN; }
int { PRINTDEBUG(yytext); return INT; }
boolean { PRINTDEBUG(yytext); return BOOL; }
public { PRINTDEBUG(yytext); return PUBLIC; }
static { PRINTDEBUG(yytext); return STATIC; }
class { PRINTDEBUG(yytext); return CLASS; }
main { PRINTDEBUG(yytext); return MAIN; }
void { PRINTDEBUG(yytext); return VOID; }
extends { PRINTDEBUG(yytext); return EXTENDS; }
length { PRINTDEBUG(yytext); return LENGTH; }
System\.out\.println { PRINTDEBUG(yytext); return PRINT_STATE_LN; }
System\.out\.print { PRINTDEBUG(yytext); return PRINT_STATE; }
if { PRINTDEBUG(yytext); PRINTDEBUG("IF"); return IF; }
else { PRINTDEBUG(yytext); return ELSE; }
while { PRINTDEBUG(yytext); return WHILE; }
this { PRINTDEBUG(yytext); return THIS; }
String { PRINTDEBUG(yytext); return STRING; }
{ID} { PRINTDEBUG(yytext); yylval.id = new Ident(new string(yytext)); return ID; }

