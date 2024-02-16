
/*
 * miniCompEval
 * Sonya Schriner
 * File: ARMv7.cpp
 *
 */

#ifdef ASSEM

#include <stdio.h>
#include <string>
#include <fstream>

#include "TreeNode.hpp"
#include "ARMv7.hpp"

using namespace std;
using namespace ARMv7;

extern ofstream assemStream;
extern Program * programRoot;

//vector< map<string, string*> * > nameTableStack;
//vector< map<string, string*> * > typeTableStack;
AssemContext * context;

void Program::assemArmv7() {
	context = new AssemContext;
  context->dataSection = new vector<string * >;
  context->textSection = new vector<string * >;
  context->textSection->push_back(new string(string(PRINTLN_EXP) + ": .asciz \"%d\\n\""));
  context->textSection->push_back(new string(string(PRINT_EXP) + ": .asciz \"%d\""));
  m->assemArmv7();
  if (c) { c->assemArmv7(); }

  // .data: global variables here
  assemStream << ".data"<< endl;
  for (auto d = context->dataSection->begin(); d < context->dataSection->end(); d++) {
    assemStream << "\t" << **d << endl;
  }
  assemStream << endl;

  // .text: write string constants here
  assemStream << ".text"<< endl;
  for (auto t = context->textSection->begin(); t < context->textSection->end(); t++) {
    assemStream << "\t" << **t << endl;
  }
  assemStream << endl;

  // .main
  assemStream << ".global main\n.balign 4\nmain:\n";
  assemStream << "\tpush {lr}\n" << endl;
  //assemStream << "\tldr r9, =" + string(PRINT_EXP) << endl;
  for (auto i = m->instr->begin(); i < m->instr->end(); i++) {
    if ((**i)[0] != '\t') { assemStream << "\t"; }
    assemStream << **i << endl;
  }
  assemStream << "\tpop {pc}" << endl;

  // .class
  if (c) {
    //assemStream << "." << << endl;
    for (auto i = c->instr->begin(); i < c->instr->end(); i++) {
      assemStream << **i ;
    }
  //  assemStream << endl;
  }
}

void MainClass::assemArmv7() {
  instr = new vector<string * >;
  string str("");
  s->assemArmv7(&str, nullptr);
  instr->push_back( new string(str) );
}

void ClassDeclList::assemArmv7() {
  for (auto c = cdVector->begin(); c < cdVector->end(); c++) {
    (*c)->assemArmv7();
  }
}

void ClassDeclSimple::assemArmv7() {
  programRoot->c->instr->push_back(new string (*(i->id) + ":\n"));
  context->nameTableStack.push_back(nameTable);
  context->typeTableStack.push_back(typeTable);
  if (v) { v->assemArmv7(nameTable, typeTable); }
  if (m) { m->assemArmv7(i->id); }
  context->typeTableStack.pop_back();
  context->nameTableStack.pop_back();
}

void ClassDeclExtends::assemArmv7() {}

void VarDeclList::assemArmv7( map<string, string *> * nameTable, map<string, string*> * typeTable) {
  for (auto v = vdVector->begin(); v < vdVector->end(); v++) {
    (*v)->assemArmv7(nameTable, typeTable);
  }
}
void VarDecl::assemArmv7( map<string, string * > * nameTable, map<string, string*> * typeTable) {
  if (dynamic_cast< PrimeType* >( t )){
    PrimeType * p = (PrimeType * ) t;
    if (dynamic_cast< BoolType* >( p )
        || dynamic_cast< IntType* >( p )) {
      context->dataSection->push_back(new string("sReg" + to_string(regCnt) + ": .word 0"));
      label = new string("sReg" + to_string(regCnt++));
      (*nameTable)[*(i->id)] = label;
      if (dynamic_cast< BoolType* >( p )) {
        (*typeTable)[*(i->id)] = new std::string("BOOL");
      } else {
        (*typeTable)[*(i->id)] = new std::string("INT");
      }
    }
  } else { // lets assume it's 1 d
    TypeIndexList * p = (TypeIndexList * ) t;
    if (dynamic_cast< BoolType* >( p->t )
        || dynamic_cast< IntType* >( p->t )) {
      context->dataSection->push_back(new string("sReg" + to_string(regCnt) + ": .word 0"));
      label = new string("sReg" + to_string(regCnt++));
      (*nameTable)[*(i->id)] = label;
      if (dynamic_cast< BoolType* >( p->t )) {
        (*typeTable)[*(i->id)] = new std::string("BOOL_A");
      } else {
        (*typeTable)[*(i->id)] = new std::string("INT_A");
      }
    }
  }
}
void MethodDecl::assemArmv7(string *objName) {
  // TODO: Make sure this is handled properly
  if (v) {v->assemArmv7(nameTable, typeTable);}

  context->nameTableStack.push_back(nameTable);
  context->typeTableStack.push_back(typeTable);

  programRoot->c->instr->push_back(new string (*objName + *(i->id) + ":\n\tpush {lr}\n\n"));
  string stmt_str("");
  s->assemArmv7(&stmt_str, nameTable);
  programRoot->c->instr->push_back(new string ("\t" + stmt_str + "\n"));
  stmt_str = string("");
  if (s) { e->assemArmv7(&stmt_str, nullptr); }
  programRoot->c->instr->push_back(new string (stmt_str + "\n\tmov r0, r1\n"));
  programRoot->c->instr->push_back(new string ("\n\tpop {pc}\n"));

  context->typeTableStack.pop_back();
  context->nameTableStack.pop_back();
}

void MethodDeclList::assemArmv7(string * objName) {
  for (auto m = mdVector->begin(); m < mdVector->end(); m++) {
    (*m)->assemArmv7(objName);
  }
}

void BlockStatements::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  s->assemArmv7(stmt_str, nameTable);
}

void IfStatement::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  string label("if" + to_string(ifCnt));
  e->assemArmv7(stmt_str, &label);

  *stmt_str = *stmt_str + "\t";
  s_if->assemArmv7(stmt_str, nameTable);

  *stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + "\tb el" + to_string(ifCnt);
  *stmt_str = *stmt_str + "\n" + label + ":\n\t";

  s_el->assemArmv7(stmt_str, nameTable);
  label = string("el" + to_string(ifCnt));
  *stmt_str = *stmt_str + label + ":\n";

  ifCnt++;
}

void WhileStatement::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  string label("lp" + to_string(lCnt) + ":\n");
  *stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + label;

  label = string("d" + to_string(lCnt) );
  e->assemArmv7(stmt_str, &label);

  *stmt_str = *stmt_str + "\t";
  s->assemArmv7(stmt_str, nameTable);
  *stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + "\tb lp" + to_string(lCnt) + "\nd" + to_string(lCnt) + ":\n";

  lCnt++;
}

void PrintLineExp::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  // need to put the format string somewhere
  string e_str("");
  e->assemArmv7(&e_str, nullptr);
  string s("ldr r0, =");
  s = e_str + "\t" + s + PRINTLN_EXP + "\n\tbl printf\n";
  *stmt_str = *stmt_str + s.substr(1);
}

void PrintLineString::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  // Adding \n to string here and creating a mem leak
  string * str = new string(*(s->str) + "\\n");
  //cout << str->c_str() << endl;
  s->str = str;

  s->assemArmv7(); // adds stuff to text section + creates label
  branchPrint(s->label, stmt_str);
}

void PrintExp::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  string e_str("");
  e->assemArmv7(&e_str, nullptr);
  string s("ldr r0, =");
  s = e_str + "\t" + s + PRINT_EXP + "\n\tbl printf\n";
  *stmt_str = *stmt_str + s.substr(1);
}

void PrintString::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  s->assemArmv7();
  branchPrint(s->label, stmt_str);
}

void Assign::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  string exp_str = "";
  e->assemArmv7(&exp_str, nullptr);
  *stmt_str = *stmt_str + exp_str.substr(1) + "\n";

  map<string, string*> * table = context->nameTableStack.back();
  if (table->find(*(i->id)) == table->end()
      && context->nameTableStack.size() > 1) {
    //cout << "table working" << endl;
    table = context->nameTableStack[context->nameTableStack.size() - 2];
  }
  string * label = (*table)[*(i->id)];
  if (label) {
    *stmt_str = *stmt_str + "\tldr r0, =" + *label + "\n";
    *stmt_str = *stmt_str + "\tstr r1, [r0]" +  "\n";
  }
}

void IndexAssign::assemArmv7(string * stmt_str, map<string, string*> *nT) {}

void ReturnStatement::assemArmv7(string * stmt_str, map<string, string*> *nT) {
  e->assemArmv7(stmt_str, nullptr);
  *stmt_str = *stmt_str + "\n\tmov r0, r1\n\tpop {pc}\n";
}

void StatementList::assemArmv7(string * stmt_str, map<string, string*> * nameTable) {
  string str("");
  for (auto s = sVector->begin(); s < sVector->end(); s++) {
    //str = str + "\t" + (*s)->assemArmv7()->c_str() + "\n";
    str = str + "\t";
    (*s)->assemArmv7(&str, nameTable);
    str = str + "\n";
  }
  *stmt_str = *stmt_str + str.substr(1);
  //return new string(str.substr(1));
}

void Or::assemArmv7(string * exp_str, string * branchLabel) {
  string label1( "e_or_S" + to_string(expCnt++) );
  string label2( "e_or_E" + to_string(expCnt++) );

  if (!branchLabel) {
    e1->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
  } else {
    e1->assemArmv7(exp_str, &label1);
    *exp_str = *exp_str + "\tb " + label2 + "\n" + label1 + ":\n";
  }

  e2->assemArmv7(exp_str, branchLabel);
  if (!branchLabel) {
    *exp_str = *exp_str + "\tpop {r2}\n\torr r1, r1, r2\n";
  } else {
    *exp_str = *exp_str + label2 + ":\n";
  }
}

void And::assemArmv7(string * exp_str, string * branchLabel) {
  string label( "e" + to_string(expCnt++) );
  e1->assemArmv7(exp_str, branchLabel);
  if (!branchLabel) {
    *exp_str = *exp_str + "\tpush {r1}\n";
  }
  e2->assemArmv7(exp_str, branchLabel);
  if (!branchLabel) {
    *exp_str = *exp_str + "\tpop {r2}\n\tand r1, r1, r2\n";
  } else {
    *exp_str = *exp_str + label + ":\n";
  }
}

void Equal::assemArmv7(string * exp_str, string * branchLabel) {
  e1->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbne " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmoveq r1, #1\n";
  }
}

void NotEqual::assemArmv7(string * exp_str, string * branchLabel) {
  e1->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbeq " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmovne r1, #1\n";
  }
}

void Lesser::assemArmv7(string * exp_str, string * branchLabel) {
  e1->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbge " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "mov r1, #0\n\tmovlt r1, #1\n";
  }
}

void Greater::assemArmv7(string * exp_str, string * branchLabel) {
    e1->assemArmv7(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assemArmv7(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n\t";
  if (branchLabel) {
    *exp_str = *exp_str + "ble " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "mov r1, #0\n\tmovgt r1, #1\n";
  }
}

void LessEqual::assemArmv7(string * exp_str, string * branchLabel) {
  e1->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assemArmv7(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbgt " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmovle r1, #1\n";
  }
}

void GreatEqual::assemArmv7(string * exp_str, string * branchLabel) {
    e1->assemArmv7(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assemArmv7(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tblt " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmovge r1, #1\n";
  }
}

void Add::assemArmv7(string * exp_str, string * branchLabel) {
    //if (dynamic_cast<LitInt *>( e1 )) {
    if (isIntLiteral(e1)) {
        std::string lit_string("");
        getIntLiteral (e1, &lit_string);
        //LitInt * expr = (LitInt *) e1;
        e2->assemArmv7(exp_str, nullptr);
        *exp_str = *exp_str + "\tadd r1, r1, " + lit_string + "\n";
        //*exp_str = *exp_str + "\tadd r1, r1, #" + to_string(expr->i->i) + "\n";
    //} else if (dynamic_cast<LitInt *>( e2 )) {
    } else if (isIntLiteral( e2 )) {
        //LitInt * expr = (LitInt *) e2;
        std::string lit_string("");
        getIntLiteral (e2, &lit_string);
        e1->assemArmv7(exp_str, nullptr);
        *exp_str = *exp_str + "\tadd r1, r1, " + lit_string + "\n";
    } else {
        e1->assemArmv7(exp_str, nullptr);
        *exp_str = *exp_str + "\tpush {r1}\n";
        e2->assemArmv7(exp_str, nullptr);
        *exp_str = *exp_str + "\tpop {r2}\n\tadd r1, r2, r1\n";
    }
}


void Subtract::assemArmv7(string * exp_str, string * branchLabel) {
  if ( isIntLiteral( e2 ) ) {
    std::string lit_string("");
    getIntLiteral (e2, &lit_string);
    e1->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tsub r1, r1, " + lit_string + "\n";
  } else if ( isIntLiteral( e1 ) ) {
    std::string lit_string("");
    getIntLiteral (e1, &lit_string);
    e2->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n\tsub r1, r2, r1\n";
  } else {
    e1->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpop {r2}\n\tsub r1, r2, r1\n";
  }
}

void Divide::assemArmv7(string * exp_str, string * branchLabel) {
    e1->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpop {r2}\n\tmov r0, #0\n";
    *exp_str = *exp_str + "\tmov r4, #0\n\tcmp r2, #0\n\tbge div" + to_string(lCnt) + "\n\tmov r4, #1\n\trsb r2, #0\n";
    *exp_str = *exp_str + "div" + to_string(lCnt) + ":\n\tcmp r2, r1\n\tblt d" + to_string(lCnt) + "\n";
    *exp_str = *exp_str + "\tsub r2, r2, r1\n\tadd r0, r0, #1\n\tb div" + to_string(lCnt) + "\n";
    *exp_str = *exp_str + "d" + to_string(lCnt) + ":\n\tmov r1, r0\n";
    *exp_str = *exp_str + "\tcmp r4, #1\n\trsbeq r1, #0\n";
    lCnt++;
}

void Multiply::assemArmv7(string * exp_str, string * branchLabel) {
  if (isIntLiteral( e1 ) && isIntLiteral( e2 )) {
    // Both are positive int literals
    string lit_string("");
    getIntLiteral (e1, &lit_string);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
    lit_string = string("");
    getIntLiteral (e2, &lit_string);
    *exp_str = *exp_str + "\tmov r1, " + lit_string + "\n";
    *exp_str = *exp_str + "\tmul r1, r2, r1\n";
  } else if (isIntLiteral( e1 )) { // op1 is a literal
    std::string lit_string("");
    getIntLiteral (e1, &lit_string);
    e2->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
    *exp_str = *exp_str + "\tmul r1, r2, r1\n";
  } else if (isIntLiteral( e2 )) { // op 2 is a literal
    std::string lit_string("");
    getIntLiteral (e2, &lit_string);
    e1->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
    *exp_str = *exp_str + "\tmul r1, r2, r1\n";
  } else { // Neither are int pos int literals
    e1->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\tpop {r2}\n\tmul r1, r2, r1\n";
  }
}

void Not::assemArmv7(string * exp_str, string * branchLabel) {
  if (dynamic_cast<True *>( e )){ // if !true = false
    // False always branches
    if (branchLabel) {
      *exp_str = *exp_str + "\tb " + *branchLabel + "\n";
    } else {
      *exp_str = *exp_str + "\tmov r0, #0\n";
    }
  } else if (dynamic_cast<False *>( e )) {  // if !false = true
    // Do Nothing - true will always execute the first part of the statement
    if (!branchLabel){
      *exp_str = *exp_str + "\tmov r0, #1\n";
    }
  } else if (dynamic_cast<Not *>( e )) { // Eliminate Assembly Generation for !!
    Not * expr = (Not * ) e;
    expr->e->assemArmv7(exp_str, branchLabel);
  } else if ( dynamic_cast< ParenExp * >( e ) ) { // if !()
    Exp * nExp = nullptr;
    ParenExp * p = (ParenExp *) e;
    nExp = p->e;
    while (dynamic_cast<ParenExp *>( nExp )) {
        ParenExp * p = (ParenExp *) nExp;
        nExp = p->e;
    }
    if (dynamic_cast<Not *>(nExp)) {
      Not * expr = (Not * ) nExp;
      expr->e->assemArmv7(exp_str, branchLabel);
    } else if (dynamic_cast<False *>( nExp )) {
      // Do Nothing - true will always execute the first part of the statement
      if (!branchLabel){
        *exp_str = *exp_str + "\tmov r0, #1\n";
      }
    } else if (dynamic_cast<True *>( nExp )) {
      // False always branches
      if (branchLabel) {
        *exp_str = *exp_str + "\tb " + *branchLabel + "\n";
      } else {
        *exp_str = *exp_str + "\tmov r0, #0\n";
      }
    } else {
      if (!branchLabel) {
        nExp->assemArmv7(exp_str, branchLabel);
        *exp_str = *exp_str + "\teor r1, r1, #1\n";
      } else {
        string label("not" + to_string(expCnt++));
        nExp->assemArmv7(exp_str, &label);
        *exp_str = *exp_str + "\tb " + *branchLabel + "\n"+ label + ":\n";
      }
    }
  } else {
    if (!branchLabel) {
      e->assemArmv7(exp_str, nullptr);
      *exp_str = *exp_str + "\teor r1, r1, #1\n";
    } else {
      string label("not" + to_string(expCnt++));
      e->assemArmv7(exp_str, &label);
      *exp_str = *exp_str + "\tb " + *branchLabel + "\n"+ label + ":\n";
    }
  }
}

void Pos::assemArmv7(string * exp_str, string * branchLabel) {
  e->assemArmv7(exp_str, nullptr);
}

void Neg::assemArmv7(string * exp_str, string * branchLabel) {
  if (dynamic_cast<LitInt *>( e )) {
    LitInt * expr = (LitInt *) e;
    *exp_str = *exp_str + "\tmov r1, #-"+ to_string(expr->i->i) + "\n";
  } else if (dynamic_cast<Neg *>( e )) {
    Neg * expr = (Neg *) e;
    expr->e->assemArmv7(exp_str, nullptr);
  } else if (dynamic_cast< ParenExp * >( e )
              || dynamic_cast< Pos * >( e ) ) { // eliminate paren

    Exp * nExp = nullptr;
    if (dynamic_cast< ParenExp * >( e )) {
      ParenExp * p = (ParenExp *) e;
      nExp = p->e;
    } else if (dynamic_cast< Pos * >( e )) {
      Pos * p = (Pos *) e;
      nExp = p->e;
    } else {
      cout << "OPPS! This is bad." << endl;
    }

    while (dynamic_cast<ParenExp *>( nExp )
        || dynamic_cast<Pos *>( nExp )) {
      if (dynamic_cast< ParenExp * >( nExp )) {
        ParenExp * p = (ParenExp *) nExp;
        nExp = p->e;
      } else if (dynamic_cast< Pos * >( nExp )) {
        Pos * p = (Pos *) nExp;
        nExp = p->e;
      } else {
        cout << "OPPS! This is bad." << endl;
      }
    }
    // There was an int in the mess
    if (dynamic_cast<LitInt *>(nExp)) {
      LitInt * expr = (LitInt *) e;
      *exp_str = *exp_str + "\tmov r1, #-"+ to_string(expr->i->i) + "\n";
    } else if (dynamic_cast<Neg *>(nExp)) { // it was another neg
      Neg * expr = (Neg *) nExp;
      expr->e->assemArmv7(exp_str, nullptr);
    } else { // something else
      nExp->assemArmv7(exp_str, nullptr);
      *exp_str = *exp_str + "\trsb r1, r1, #0\n";
    }
  } else {
    e->assemArmv7(exp_str, nullptr);
    *exp_str = *exp_str + "\trsb r1, r1, #0\n";
    //*exp_str = *exp_str + "\tmov r2, #-1\n\tmul r1, r1, r2\n";
  }
}

void ParenExp::assemArmv7(string * exp_str, string * branchLabel) {
  e->assemArmv7(exp_str, branchLabel);
}

void LitInt::assemArmv7(string * exp_str, string * branchLabel) {
  *exp_str = *exp_str + "\tmov r1, #"+ to_string(i->i) + "\n";
}

void True::assemArmv7(string * exp_str, string * branchLabel) {
  // Do Nothing - true will always execute the first part of the statement
  if (branchLabel == nullptr){
    *exp_str = *exp_str + "\tmov r1, #1\n";
  }
}

void False::assemArmv7(string * exp_str, string * branchLabel) {
  // False always branches
  if (branchLabel == nullptr){
    *exp_str = *exp_str + "\tmov r1, #0\n";
  } else {
    *exp_str = *exp_str + "\tb " + *branchLabel + "\n";
  }
}

void ExpObject::assemArmv7(string * exp_str, string * branchLabel) {
  o->assemArmv7(exp_str, branchLabel);
}

void ObjectMethodCall::assemArmv7(string * exp_str, string * branchLabel) {
  if ( dynamic_cast<NewIdObj * >(o) ) {
    NewIdObj * obj = (NewIdObj * ) o;
    *exp_str = *exp_str + "\tbl " + *(obj->i->id) + *(i->id) + "\n";
    *exp_str = *exp_str + "\tmov r1, r0\n";
  } else if ( dynamic_cast<IdObj * >(o) ) {
    IdObj * obj = (IdObj * ) o;
    *exp_str = *exp_str + "\tbl " + *(obj->i->id) + *(i->id) + "\n";
    *exp_str = *exp_str + "\tmov r1, r0\n";
  }
}

void IdObj::assemArmv7(string * exp_str, string * branchLabel) {
  map<string, string*> * table = context->nameTableStack.back();
  if (table->find(*(i->id)) == table->end()
      && context->nameTableStack.size() > 1) {
    table = context->nameTableStack[context->nameTableStack.size() - 2];
  }
  string * label = (*table)[*(i->id)];
  if (label) {
    *exp_str = *exp_str + "\tldr r0, =" + *label + "\n";
    *exp_str = *exp_str + "\tldr r1, [r0]" + "\n";
  }
  //*stmt_str = *stmt_str + "\tstr r1, [r0]" +  "\n";
}

void NewTypeObj::assemArmv7(string * exp_str, string * branchLabel) {
  /*
  map<string, string*> * table = context->nameTableStack.back();
  if (table->find(*(i->id)) == table->end()
      && context->nameTableStack.size() > 1) {
    //cout << "table working" << endl;
    table = context->nameTableStack[context->nameTableStack.size() - 2];
  }
  string * label = (*table)[*(i->id)];
  if (label) {
    *exp_str = *exp_str + "\tldr r0, =" + *label + "\n";
    *exp_str = *exp_str + "\tldr r1, [r0]" + "\n";
  }*/
}

void StringLiteral::assemArmv7() {
  context->textSection->push_back(new string("str" + to_string(strCnt) + ": .asciz \"" + str->c_str() + "\""));
  label = new string("str" + to_string(strCnt++));
}

#endif
