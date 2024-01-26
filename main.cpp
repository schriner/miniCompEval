
#include <stdio.h>
#include <string>
#include <fstream>
#include "TreeNode.hpp"

int yyparse(void);

using namespace std;

fstream assemStream;
Program * programRoot = NULL;
TableNode * rootScope = NULL;
bool programTypeError = false;

/* Main */
int main( int argc, char** argv ) {
	extern FILE * yyin;
	
	// Pass fp to yy
	if (argc > 1) {
		// Create Assembly File
		string assemFileName(argv[1]);
		assemFileName = assemFileName.substr(0, assemFileName.find(".java"));	
		assemFileName = assemFileName + ".s";
		assemStream.open(assemFileName, fstream::out | fstream::trunc);
		
		FILE * fp = fopen(argv[1], "r");
		yyin = fp;
		yyparse();

		//programRoot->traverse();
		//if (!programTypeError) { programRoot->evaluate(); }
		if (!programTypeError) { programRoot->assem(); }

		assemStream.close();
	} else {
		yyparse();
	}
}
