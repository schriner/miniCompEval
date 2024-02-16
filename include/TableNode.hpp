
/*
 *
 * miniCompEval
 * Sonya Schriner
 * File: TableNode.hpp
 *
 */

#ifndef TABLENODE_HPP
#define TABLENODE_HPP
#include <map>
#include <vector>

using std::string;
using std::map;

namespace symTab {

template <typename Type>
struct _SYMBOL;

// TODO(ss): use this for compairison operations within map<string, SYM> and update BLOCK_SCOPE and call_stack
struct StringCmp {
  bool operator()(string * lhs, string * rhs) const {
    return !lhs->compare(*rhs);
  }
};

template <typename Type>
union _ID_ARRAY {
	map<string, _SYMBOL<Type>> * id;
	int index;
}; 

template <typename Type>
union _VALUE {
  int exp; // bool or int
  int * exp_array; // bool or int single indx array
  map<string, _SYMBOL<Type>> * id; // identifier type
  _ID_ARRAY<Type> * id_array; // identifier type array
  string * name;  // classname
};

template <typename Type>
struct _SYMBOL {
  _VALUE<Type> val;
  Type * t;
};

template <typename Type>
struct _BLOCK_SCOPE {
  map<string, _SYMBOL<Type>> table;
  _BLOCK_SCOPE<Type> * parent;
  const typename map<string, _SYMBOL<Type>>::iterator end() {
    return table.end();
  }
  typename map<string, _SYMBOL<Type>>::iterator find(string id) {
    _BLOCK_SCOPE<Type> * s = this;
    while (s) {
      if (s->table.find(id) != s->table.end()) {
        return s->table.find(id);
      }
      s = s->parent;
    }
    return end();
  }
  _SYMBOL<Type>& operator[](const string id) {
    return find(id)->second;
  }
	  void insert(const string id, _SYMBOL<Type> s) {
    table[id] = s;
  }
  _BLOCK_SCOPE(_BLOCK_SCOPE * p)
    : parent(p) {}
  _BLOCK_SCOPE()
    : parent(nullptr){}
};

// Define Evaluate Context Here

}

#endif
