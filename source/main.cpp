
#include <stdio.h>
#include <string>
#include <fstream>
#include <cassert>

#include "TreeNode.hpp"

#define INPUT_FORMAT "Expected: mjavac [-o <output_file> | --stdout ] <inputfile>.java"
#define PARSING_ERROR(x) fprintf(stderr, "\nmjavac: %s\n%s\n\n", (x), INPUT_FORMAT);

int yyparse(void);

using namespace std;

ofstream assemStream;
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
		
		string assemFileName(argv[2]);
		assemStream.open(assemFileName, fstream::out | fstream::trunc);
		
		FILE * fp = fopen(argv[3], "r");
		yyin = fp;
		yyparse();

		// Generate Assembly :
		if (!programTypeError) { programRoot->assem(); }

		assemStream.close();

	} else if (argc == 3 && string(argv[2]).find(".java") != string::npos) {
		if (!string("--stdout").compare(argv[1])) {
			assemStream.basic_ios<char>::rdbuf(cout.rdbuf());
			
			FILE * fp = fopen(argv[2], "r");

			yyin = fp;
			yyparse();

			// Generate Assembly : 
			if (!programTypeError) { programRoot->assem(); }

			//assemStream.close();
		} else {
			PARSING_ERROR("invalid flag");
		}
	} else if (argc == 2 && string(argv[1]).find(".java") != string::npos) {
		
		// Create Assembly File
		string assemFileName(argv[1]);
		assemFileName = assemFileName.substr(0, assemFileName.find(".java"));	
		assemFileName = assemFileName + ".s";
		assemStream.open(assemFileName, fstream::out | fstream::trunc);
		
		FILE * fp = fopen(argv[1], "r");
		yyin = fp;
		yyparse();

		// TODO(ss): Do something with this
		// Traverse AST : 
		// programRoot->traverse();
		
		// Generate Assembly : 
		if (!programTypeError) { programRoot->assem(); }

		assemStream.close();
	} else if (argc == 1){
		
		// Parse stdin
		yyparse();

	} else {
		PARSING_ERROR("invalid argument format");
	}

}
