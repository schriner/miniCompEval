
/*
 * miniCompEval
 * Sonya Schriner
 * File: mjavacllvm.cpp
 *
 */

#include <stdio.h>
#include <string>
#include <fstream>
#include "TreeNode.hpp"


#define INPUT_FORMAT "Expected: mjavacllvm [ -o <output_file> | --stdout ] <inputfile>.java"
#define PARSING_ERROR(x) { fprintf(stderr, "%s: %s\n%s\n\n", exe, (x), INPUT_FORMAT); abort(); }

int yyparse(void);

using namespace std;
Program * programRoot = nullptr;
bool programTypeError = false;
string bcFilename;

char * yyfilename = nullptr;
char * exe;

void GenerateIR(Program * root);

/* Main */
int main( int argc, char** argv ) {
	extern FILE * yyin;

	exe = argv[0];

	// Pass fp to yy
	if (argc == 4 && !string("-o").compare(argv[1])
			&& string(argv[3]).find(".java") != string::npos
			&& string(argv[2]).find(".java") == string::npos) {

		bcFilename = string(argv[2]);
		//bcStream.open(bcFilename, fstream::out | fstream::trunc);

		yyin = fopen(argv[3], "r");
		yyfilename = argv[3];

	} else if (argc == 3 && string(argv[2]).find(".java") != string::npos) {
		if (!string("--stdout").compare(argv[1])) {
			yyin = fopen(argv[2], "r");
			yyfilename = argv[2];

		} else {
			PARSING_ERROR("invalid flag");
		}

	} else if (argc == 2 && string(argv[1]).find(".java") != string::npos) { 
		// Create Bitcode File
		bcFilename = string(argv[1]);
		bcFilename = bcFilename.substr(0, bcFilename.find(".java"));
		bcFilename = bcFilename + ".bc";
		//bcStream.open(bcFilename, fstream::out | fstream::trunc);
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

	GenerateIR(programRoot);
	
}
