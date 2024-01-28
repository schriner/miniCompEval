
/*
 * miniCompEval
 * Sonya Schriner
 * File: TreeNode.cpp
 *
 */

#include <string>

using namespace std;

string * ExpResult = nullptr;
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

