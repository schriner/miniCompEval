
/*
 * miniCompEval
 * Sonya Schriner
 * File: util_assem.cpp
 *
 */

#include "TreeNode.hpp"

using namespace std;

class Exp;
class LitInt;
class ParenExp;
class Neg;
class Pos;

void branchPrint(string * str, string * stmt_str){
  string s("ldr r0, =");
  s = s + str->c_str() + "\n\tbl printf\n";
  *stmt_str = *stmt_str + s;
}

bool isIntLiteral (Exp * e) {
  if (dynamic_cast<LitInt *>( e )) { // op1 is a literal
    return true;
  }
  if (dynamic_cast<ParenExp *>( e )) { // check contents of parentheses
    ParenExp * expr = (ParenExp * ) e;
    return isIntLiteral(expr->e);
  }
  if (dynamic_cast<Neg *> ( e )) { // check the negative
    Neg * expr = (Neg * ) e;
    return isIntLiteral(expr->e);
  }
  if (dynamic_cast<Pos *> ( e )) {
    Pos * expr = (Pos * ) e;
    return isIntLiteral(expr->e);
  }
  return false; // expr is false
}

void getIntLiteral (Exp * e, string * s) {
  if (dynamic_cast<LitInt *>( e )) { // op1 is a literal
    LitInt * expr = (LitInt *) e;
    *s = *s + "#" + to_string(expr->i->i);
    return;
  }

  int negCnt = 0;
  while (dynamic_cast<ParenExp *>( e ) || dynamic_cast<Neg *> ( e )
          || dynamic_cast<LitInt *>( e ) || dynamic_cast<Pos *> ( e )) {
    if (dynamic_cast<ParenExp *>( e )) { // ignore parentheses
      ParenExp * expr = (ParenExp * ) e;
      e = expr->e;
    } else if (dynamic_cast< Pos * >( e )) {
      Pos * expr = (Pos * ) e;
      e = expr->e;
    } else if (dynamic_cast< Neg * > ( e )) { // count the negative
      negCnt++;
      Neg * expr = (Neg * ) e;
      e = expr->e;
    } else if (dynamic_cast<LitInt *>( e )) {
      LitInt * expr = (LitInt *) e;
      if (negCnt % 2) {
        *s = *s + "#-" + to_string(expr->i->i);
      } else {
        *s = *s + "#" + to_string(expr->i->i);
      }

      // stop when you hit the literal
      return;
    }
  }

	cerr << "Error processing integer literal for assembly generation, lineno: " << e->lineRecord <<endl;

}
