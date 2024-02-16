
/*
 *
 * miniCompEval
 * Sonya Schriner
 * File: Evaluate.hpp
 *
 */

#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include <vector>
#include "TableNode.hpp"

using std::string;
using std::map;
using std::vector;

class Type;
class ExpList;

namespace Interpreter {

using ID_ARRAY = symTab::_ID_ARRAY<Type>;
using VAL = symTab::_VALUE<Type>;
using SYM = symTab::_SYMBOL<Type>;
using SCOPE = symTab::_BLOCK_SCOPE<Type>;

struct InterpreterContext {
  // merge these
  vector<map<string, SYM> *> call_stack;
  vector<SCOPE *> scope_stack{new SCOPE()};

  VAL return_reg; // std::optional
  ExpList * arg_stack = nullptr;
  //TODO: std::optional<VAL> return_reg;
  //TODO: std::optional<ExpList *> arg_stack = nullptr;

  void push_nested_scope() {
    SCOPE * scope = new SCOPE(scope_stack.back());
    scope_stack[scope_stack.size() - 1] = scope;
  }

  void pop_nested_scope() {
    auto tmp = scope_stack.back();
    scope_stack[scope_stack.size() - 1] = scope_stack[scope_stack.size() - 1]->parent;
    delete tmp;
  }

	~InterpreterContext(){
		delete scope_stack.back();
	}
};

} // namespace

#endif
