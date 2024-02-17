
/*
 * miniCompEval
 * Sonya Schriner
 * File: LLVM.cpp
 *
 */


#include "TreeNode.hpp"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace std;
using namespace llvm;

void GenerateIR() {
	LLVMContext Context;

  // Create some module to put our function into it.
	std::unique_ptr<Module> Owner(new Module("test", Context));
	Module *M = Owner.get();

}
