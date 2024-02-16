
/*
 * miniCompEval
 * Sonya Schriner
 * File: TreeNode.cpp
 *
 */

#include <stdio.h>
#include <string>
#include <fstream>

#include "TreeNode.hpp"

#define INPUT_FORMAT "Expected: mjavac [ -p | -o <output_file> | --stdout ] <inputfile>.java"
#define PARSING_ERROR(x) { fprintf(stderr, "mjavac: %s\n%s\n\n", (x), INPUT_FORMAT); abort(); }

int yyparse(void);

using namespace std;

ofstream assemStream;
Program * programRoot = nullptr;
char * yyfilename = nullptr;
bool programTypeError = false;
char * exe;

/* Main */
int main( int argc, char** argv ) {
	extern FILE * yyin;

	exe = argv[0];

	// Pass fp to yy
	if (argc == 3 && !string("-p").compare(argv[1])
				&& string(argv[2]).find(".java") != string::npos) {
			
			yyin = fopen(argv[2], "r");
			yyfilename = argv[2];
			if (!yyin) { PARSING_ERROR(".java filepath not found"); }
			yyparse();
			return 0;
	
	} else if (argc == 4 && !string("-o").compare(argv[1])
			&& string(argv[3]).find(".java") != string::npos
			&& string(argv[2]).find(".java") == string::npos) {
		
		string assemFileName(argv[2]);
		assemStream.open(assemFileName, fstream::out | fstream::trunc);
		
		yyin = fopen(argv[3], "r");
		yyfilename = argv[3];

	} else if (argc == 3 && string(argv[2]).find(".java") != string::npos) {
		if (!string("--stdout").compare(argv[1])) {
			assemStream.basic_ios<char>::rdbuf(cout.rdbuf());
			
			yyin = fopen(argv[2], "r");
			yyfilename = argv[2];

		} else {
			PARSING_ERROR("invalid flag");
		}
	} else if (argc == 2 && string(argv[1]).find(".java") != string::npos) {
		
		// Create Assembly File
		string assemFileName(argv[1]);
		assemFileName = assemFileName.substr(0, assemFileName.find(".java"));	
		assemFileName = assemFileName + ".s";
		assemStream.open(assemFileName, fstream::out | fstream::trunc);
		
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

	// Generate Assembly : 
	programRoot->assemArmv7();
	if (argc != 3) assemStream.close();

}
