
#include <stdio.h>
#include <fstream>
#include <string>
#include <cassert>

#include "TreeNode.hpp"

int yyparse(void);

using namespace std;

Program * programRoot = NULL;
TableNode * rootScope = NULL;
bool programTypeError = false;

/* Main */
int main( int argc, char** argv ) {
	extern FILE * yyin;
	
	// Pass fp to yy
	if (argc == 4 && !string("-o").compare(argv[1])
			&& string(argv[3]).find(".java") != string::npos
			&& string(argv[2]).find(".java") == string::npos) {
		
		ofstream file(argv[2]);
		streambuf * old = cout.rdbuf(file.rdbuf());
		
		FILE * fp = fopen(argv[3], "r");
		yyin = fp;
		yyparse();
		
		// Evaluate Expressions (Interpreter): 
		if (!programTypeError) { programRoot->evaluate(); }

		cout.rdbuf(old); // restore

	} else if (argc == 2 && string(argv[1]).find(".java") != string::npos) {
		
		FILE * fp = fopen(argv[1], "r");
		yyin = fp;
		yyparse();

		// Traverse AST : 
		// programRoot->traverse();
		
		// Evaluate Expressions (Interpreter): 
		if (!programTypeError) { programRoot->evaluate(); }
		
	}  else {
		cerr << "meval: invalid argument format" << endl;
		cerr << "Expected: meval [-o <output_file>] <inputfile>.java" << endl;
	}

}
