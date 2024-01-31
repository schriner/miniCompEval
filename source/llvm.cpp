
/*
 * miniCompEval
 * Sonya Schriner
 * File: llvm.cpp
 *
 */

#include <stdio.h>
#include <string>
#include <fstream>


#define INPUT_FORMAT "Expected: mjavac_llvm <inputfile>.java"//[ -p | -o <output_file> | --stdout ] <inputfile>.java"
#define PARSING_ERROR(x) { fprintf(stderr, "mjavac: %s\n%s\n\n", (x), INPUT_FORMAT); abort(); }

int yyparse(void);

using namespace std;

char * yyfilename = nullptr;
char * exe;

/* Main */
int main( int argc, char** argv ) {
	extern FILE * yyin;

	exe = argv[0];

	// Pass fp to yy
	if (argc == 2 && string(argv[1]).find(".java") != string::npos) {
		yyin = fopen(argv[1], "r");
		yyfilename = argv[2];

	} else if (argc == 1){
		
		// Parse stdin
		yyparse();
		return 0;

	} else {
		PARSING_ERROR("invalid argument format");
	}

	if (!yyin) { PARSING_ERROR(".java filepath not found"); }
	yyparse();

}
