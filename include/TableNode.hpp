
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

class Type;
struct _SYM;
using std::string;
using std::map;

// TODO(ss): use this for compairison operations within map<string, SYM> and update SCOPE and call_stack
struct StringCmp {
  bool operator()(string * lhs, string * rhs) const {
    return !lhs->compare(*rhs);
  }
};

typedef union _ID_ARRAY {
  map<string, _SYM> * id;
  int index;
} ID_ARRAY;

typedef union _VAL {
  int exp; // bool or int
  int * exp_array; // bool or int single indx array
  map<string, _SYM> * id; // identifier type
  _ID_ARRAY * id_array; // identifier type array
  string * name;  // classname
} VAL;

typedef struct _SYM {
  VAL val;
  Type * t;
} SYM;

typedef struct _SCOPE {
  map<string, SYM> table;
  _SCOPE * parent;
  const map<string, SYM>::iterator end() {
    return table.end();
  }
  map<string, SYM>::iterator find(string id) {
    _SCOPE * s = this;
    while (s) {
      if (s->table.find(id) != s->table.end()) {
        return s->table.find(id);
      }
      s = s->parent;
    }
    return end();
  }
  SYM& operator[](const string id) {
    return find(id)->second;
  }
	  void insert(const string id, SYM s) {
    table[id] = s;
  }
  _SCOPE(_SCOPE * p)
    : parent(p) {}
  _SCOPE()
    : parent(nullptr){}
} SCOPE;

#endif
