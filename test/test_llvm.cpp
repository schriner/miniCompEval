
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <filesystem>

using namespace std;

int main(int argc, char ** argv) {
	chdir("../..");
	string s = std::filesystem::current_path().string() + "/test/test_llvm.sh";
	char * arg[] = {strdup("/bin/sh"), strdup(s.c_str()), argv[1], NULL};
	execv("/bin/sh", arg);
}
