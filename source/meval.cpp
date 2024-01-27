
#include <stdio.h>
#include <fstream>
#include <string>
#include <cassert>

#include "TreeNode.hpp"

#define INPUT_FORMAT "Expected: meval [-o <output_file>] <inputfile>.java"
#define PARSING_ERROR(x) { fprintf(stderr, "meval: %s\n%s\n\n", (x), INPUT_FORMAT); abort(); }

int yyparse(void);

using namespace std;

Program * programRoot = nullptr;
TableNode * rootScope = nullptr;
bool programTypeError = false;

/* Main */
int main( int argc, char** argv ) {
	extern FILE * yyin;
	streambuf * old;

	// Pass fp to yy
	if (argc == 4 && !string("-o").compare(argv[1])
			&& string(argv[3]).find(".java") != string::npos
			&& string(argv[2]).find(".java") == string::npos) {
		
		ofstream file(argv[2], fstream::trunc);
		old = cout.rdbuf(file.rdbuf());
		yyin = fopen(argv[3], "r");

	} else if (argc == 2 && string(argv[1]).find(".java") != string::npos) {
		yyin = fopen(argv[1], "r");
		
	} else {
		PARSING_ERROR("invalid argument format");
	}
	
	if (!yyin) { PARSING_ERROR(".java filepath not found"); }
	yyparse();

	// Evaluate Expressions (Interpreter): 
	programRoot->evaluate();

	if (argc == 4) cout.rdbuf(old);
}
