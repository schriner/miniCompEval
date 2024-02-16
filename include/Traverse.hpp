
/*
 * miniCompEval
 * Sonya Schriner
 * File: Traverse.hpp
 *
 */

#ifndef TRAVERSE_HPP
#define TRAVERSE_HPP

#include <string>

using namespace std;

namespace traverse {

string * ExpResult = nullptr;


const char * b_res = "BOOL";
const char * i_res = "INT";
const char * o_res = "OBJ";
bool isBool = false;
bool expErr = false;

void updateExpResult (const char * r) {
  if (ExpResult) { delete ExpResult; }
  ExpResult = new string(r);
}
void updateExpResult (string * r) {
  if (ExpResult) { delete ExpResult; }
  ExpResult = new string(*r);
}
bool checkExpResult (const char * exp) {
  return ExpResult->compare(exp);
}
bool checkExpResult (string * r) {
  return ExpResult->compare(*r);
}

}; // namespace evaluate
#endif
