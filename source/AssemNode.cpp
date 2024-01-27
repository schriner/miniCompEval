
#ifdef ASSEM

#include <stdio.h>
#include <string>
#include <fstream>

#include "TreeNode.hpp"

using namespace std;

extern ofstream assemStream;
extern Program * programRoot;


int ifCnt = 0;
int lCnt = 0;
int strCnt = 0;
int expCnt = 0;
int regCnt = 0;

vector< map<string, string*> * > nameTableStack;
vector< map<string, string*> * > typeTableStack;

void branchPrint(string * str, string * stmt_str);
bool isIntLiteral(Exp * e);
void getIntLiteral (Exp * e, string * s);

void Program::assem() {
  dataSection = new vector<string * >;
  textSection = new vector<string * >;
  textSection->push_back(new string(string(PRINTLN_EXP) + ": .asciz \"%d\\n\""));
  textSection->push_back(new string(string(PRINT_EXP) + ": .asciz \"%d\""));
  m->assem();
  if (c) { c->assem(); }

  // .data: global variables here
  assemStream << ".data"<< endl;
  for (auto d = dataSection->begin(); d < dataSection->end(); d++) {
    assemStream << "\t" << **d << endl;
  }
  assemStream << endl;

  // .text: write string constants here
  assemStream << ".text"<< endl;
  for (auto t = textSection->begin(); t < textSection->end(); t++) {
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

void MainClass::assem() {
  instr = new vector<string * >;
  string str("");
  s->assem(&str, nullptr);
  instr->push_back( new string(str) );
}

void ClassDeclList::assem() {
  for (auto c = cdVector->begin(); c < cdVector->end(); c++) {
    (*c)->assem();
  }
}

void ClassDeclSimple::assem() {
  programRoot->c->instr->push_back(new string (*(i->id) + ":\n"));
  nameTableStack.push_back(nameTable);
  typeTableStack.push_back(typeTable);
  if (v) { v->assem(nameTable, typeTable); }
  if (m) { m->assem(i->id); }
  typeTableStack.pop_back();
  nameTableStack.pop_back();
}

void ClassDeclExtends::assem() {}

void VarDeclList::assem( map<string, string *> * nameTable, map<string, string*> * typeTable) {
  for (auto v = vdVector->begin(); v < vdVector->end(); v++) {
    (*v)->assem(nameTable, typeTable);
  }
}
void VarDecl::assem( map<string, string * > * nameTable, map<string, string*> * typeTable) {
  if (dynamic_cast< TypePrime* >( t )){
    TypePrime * p = (TypePrime * ) t;
    if (dynamic_cast< BoolType* >( p->p )
        || dynamic_cast< IntType* >( p->p )) {
      programRoot->dataSection->push_back(new string("sReg" + to_string(regCnt) + ": .word 0"));
      label = new string("sReg" + to_string(regCnt++));
      (*nameTable)[*(i->id)] = label;
      if (dynamic_cast< BoolType* >( p->p )) {
        (*typeTable)[*(i->id)] = new std::string("BOOL");
      } else {
        (*typeTable)[*(i->id)] = new std::string("INT");
      }
    }
  } else { // lets assume it's 1 d
    TypeIndexList * p = (TypeIndexList * ) t;
    if (dynamic_cast< BoolType* >( p->t )
        || dynamic_cast< IntType* >( p->t )) {
      programRoot->dataSection->push_back(new string("sReg" + to_string(regCnt) + ": .word 0"));
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
void MethodDecl::assem(string *objName) {
  // TODO: Make sure this is handled properly
  if (v) {v->assem(nameTable, typeTable);}

  nameTableStack.push_back(nameTable);
  typeTableStack.push_back(typeTable);

  programRoot->c->instr->push_back(new string (*objName + *(i->id) + ":\n\tpush {lr}\n\n"));
  string stmt_str("");
  s->assem(&stmt_str, nameTable);
  programRoot->c->instr->push_back(new string ("\t" + stmt_str + "\n"));
  stmt_str = string("");
  if (s) { e->assem(&stmt_str, nullptr); }
  programRoot->c->instr->push_back(new string (stmt_str + "\n\tmov r0, r1\n"));
  programRoot->c->instr->push_back(new string ("\n\tpop {pc}\n"));

  typeTableStack.pop_back();
  nameTableStack.pop_back();
}

void MethodDeclList::assem(string * objName) {
  for (auto m = mdVector->begin(); m < mdVector->end(); m++) {
    (*m)->assem(objName);
  }
}

void BlockStatements::assem(string * stmt_str, map<string, string*> * nameTable)  {
  s->assem(stmt_str, nameTable);
}

void IfStatement::assem(string * stmt_str, map<string, string*> * nameTable)  {
  string label("if" + to_string(ifCnt));
  e->assem(stmt_str, &label);

  *stmt_str = *stmt_str + "\t";
  s_if->assem(stmt_str, nameTable);

  *stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + "\tb el" + to_string(ifCnt);
  *stmt_str = *stmt_str + "\n" + label + ":\n\t";

  s_el->assem(stmt_str, nameTable);
  label = string("el" + to_string(ifCnt));
  *stmt_str = *stmt_str + label + ":\n";

  ifCnt++;
}

void WhileStatement::assem(string * stmt_str, map<string, string*> * nameTable)  {
  string label("lp" + to_string(lCnt) + ":\n");
  *stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + label;

  label = string("d" + to_string(lCnt) );
  e->assem(stmt_str, &label);

  *stmt_str = *stmt_str + "\t";
  s->assem(stmt_str, nameTable);
  *stmt_str = stmt_str->substr(0, stmt_str->length() - 1) + "\tb lp" + to_string(lCnt) + "\nd" + to_string(lCnt) + ":\n";

  lCnt++;
}

void PrintLineExp::assem(string * stmt_str, map<string, string*> * nameTable){
  // need to put the format string somewhere
  string e_str("");
  e->assem(&e_str, nullptr);
  string s("ldr r0, =");
  s = e_str + "\t" + s + PRINTLN_EXP + "\n\tbl printf\n";
  *stmt_str = *stmt_str + s.substr(1);
}

void PrintLineString::assem(string * stmt_str, map<string, string*> * nameTable) {
  // Adding \n to string here and creating a mem leak
  string * str = new string(*(s->str) + "\\n");
  //cout << str->c_str() << endl;
  s->str = str;

  s->assem(); // adds stuff to text section + creates label
  branchPrint(s->label, stmt_str);
}

void PrintExp::assem(string * stmt_str, map<string, string*> * nameTable){
  string e_str("");
  e->assem(&e_str, nullptr);
  string s("ldr r0, =");
  s = e_str + "\t" + s + PRINT_EXP + "\n\tbl printf\n";
  *stmt_str = *stmt_str + s.substr(1);
}

void PrintString::assem(string * stmt_str, map<string, string*> * nameTable){
  s->assem();
  branchPrint(s->label, stmt_str);
}

void Assign::assem(string * stmt_str, map<string, string*> * nameTable) {
  string exp_str = "";
  e->assem(&exp_str, nullptr);
  *stmt_str = *stmt_str + exp_str.substr(1) + "\n";

  map<string, string*> * table = nameTableStack.back();
  if (table->find(*(i->id)) == table->end()
      && nameTableStack.size() > 1) {
    //cout << "table working" << endl;
    table = nameTableStack[nameTableStack.size() - 2];
  }
  string * label = (*table)[*(i->id)];
  if (label) {
    *stmt_str = *stmt_str + "\tldr r0, =" + *label + "\n";
    *stmt_str = *stmt_str + "\tstr r1, [r0]" +  "\n";
  }
}

void IndexAssign::assem(string * stmt_str, map<string, string*> *nT) {}

void ReturnStatement::assem(string * stmt_str, map<string, string*> *nT){
  e->assem(stmt_str, nullptr);
  *stmt_str = *stmt_str + "\n\tmov r0, r1\n\tpop {pc}\n";
}

void StatementList::assem(string * stmt_str, map<string, string*> * nameTable) {
  string str("");
  for (auto s = sVector->begin(); s < sVector->end(); s++) {
    //str = str + "\t" + (*s)->assem()->c_str() + "\n";
    str = str + "\t";
    (*s)->assem(&str, nameTable);
    str = str + "\n";
  }
  *stmt_str = *stmt_str + str.substr(1);
  //return new string(str.substr(1));
}

void Or::assem(string * exp_str, string * branchLabel) {
  string label1( "e_or_S" + to_string(expCnt++) );
  string label2( "e_or_E" + to_string(expCnt++) );

  if (!branchLabel) {
    e1->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
  } else {
    e1->assem(exp_str, &label1);
    *exp_str = *exp_str + "\tb " + label2 + "\n" + label1 + ":\n";
  }

  e2->assem(exp_str, branchLabel);
  if (!branchLabel) {
    *exp_str = *exp_str + "\tpop {r2}\n\torr r1, r1, r2\n";
  } else {
    *exp_str = *exp_str + label2 + ":\n";
  }
}

void And::assem(string * exp_str, string * branchLabel){
  string label( "e" + to_string(expCnt++) );
  e1->assem(exp_str, branchLabel);
  if (!branchLabel) {
    *exp_str = *exp_str + "\tpush {r1}\n";
  }
  e2->assem(exp_str, branchLabel);
  if (!branchLabel) {
    *exp_str = *exp_str + "\tpop {r2}\n\tand r1, r1, r2\n";
  } else {
    *exp_str = *exp_str + label + ":\n";
  }
}

void Equal::assem(string * exp_str, string * branchLabel){
  e1->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbne " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmoveq r1, #1\n";
  }
}

void NotEqual::assem(string * exp_str, string * branchLabel){
  e1->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbeq " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmovne r1, #1\n";
  }
}

void Lesser::assem(string * exp_str, string * branchLabel){
  e1->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbge " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "mov r1, #0\n\tmovlt r1, #1\n";
  }
}

void Greater::assem(string * exp_str, string * branchLabel){
    e1->assem(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assem(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n\t";
  if (branchLabel) {
    *exp_str = *exp_str + "ble " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "mov r1, #0\n\tmovgt r1, #1\n";
  }
}

void LessEqual::assem(string * exp_str, string * branchLabel){
  e1->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpush {r1}\n";
  e2->assem(exp_str, nullptr); // has to b int
  *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tbgt " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmovle r1, #1\n";
  }
}

void GreatEqual::assem(string * exp_str, string * branchLabel){
    e1->assem(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assem(exp_str, nullptr); // has to b int
    *exp_str = *exp_str + "\tpop {r2}\n\tcmp r2, r1\n";
  if (branchLabel) {
    *exp_str = *exp_str + "\tblt " + *branchLabel + "\n";
  } else {
    *exp_str = *exp_str + "\tmov r1, #0\n\tmovge r1, #1\n";
  }
}

void Add::assem(string * exp_str, string * branchLabel){
    //if (dynamic_cast<LitInt *>( e1 )) {
    if (isIntLiteral(e1)) {
        std::string lit_string("");
        getIntLiteral (e1, &lit_string);
        //LitInt * expr = (LitInt *) e1;
        e2->assem(exp_str, nullptr);
        *exp_str = *exp_str + "\tadd r1, r1, " + lit_string + "\n";
        //*exp_str = *exp_str + "\tadd r1, r1, #" + to_string(expr->i->i) + "\n";
    //} else if (dynamic_cast<LitInt *>( e2 )) {
    } else if (isIntLiteral( e2 )) {
        //LitInt * expr = (LitInt *) e2;
        std::string lit_string("");
        getIntLiteral (e2, &lit_string);
        e1->assem(exp_str, nullptr);
        *exp_str = *exp_str + "\tadd r1, r1, " + lit_string + "\n";
    } else {
        e1->assem(exp_str, nullptr);
        *exp_str = *exp_str + "\tpush {r1}\n";
        e2->assem(exp_str, nullptr);
        *exp_str = *exp_str + "\tpop {r2}\n\tadd r1, r2, r1\n";
    }
}


void Subtract::assem(string * exp_str, string * branchLabel){
  if ( isIntLiteral( e2 ) ) {
    std::string lit_string("");
    getIntLiteral (e2, &lit_string);
    e1->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tsub r1, r1, " + lit_string + "\n";
  } else if ( isIntLiteral( e1 ) ) {
    std::string lit_string("");
    getIntLiteral (e1, &lit_string);
    e2->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n\tsub r1, r2, r1\n";
  } else {
    e1->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpop {r2}\n\tsub r1, r2, r1\n";
  }
}

void Divide::assem(string * exp_str, string * branchLabel) {
    e1->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpop {r2}\n\tmov r0, #0\n";
    *exp_str = *exp_str + "\tmov r4, #0\n\tcmp r2, #0\n\tbge div" + to_string(lCnt) + "\n\tmov r4, #1\n\trsb r2, #0\n";
    *exp_str = *exp_str + "div" + to_string(lCnt) + ":\n\tcmp r2, r1\n\tblt d" + to_string(lCnt) + "\n";
    *exp_str = *exp_str + "\tsub r2, r2, r1\n\tadd r0, r0, #1\n\tb div" + to_string(lCnt) + "\n";
    *exp_str = *exp_str + "d" + to_string(lCnt) + ":\n\tmov r1, r0\n";
    *exp_str = *exp_str + "\tcmp r4, #1\n\trsbeq r1, #0\n";
    lCnt++;
}

void Multiply::assem(string * exp_str, string * branchLabel){
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
    e2->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
    *exp_str = *exp_str + "\tmul r1, r2, r1\n";
  } else if (isIntLiteral( e2 )) { // op 2 is a literal
    std::string lit_string("");
    getIntLiteral (e2, &lit_string);
    e1->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tmov r2, " + lit_string + "\n";
    *exp_str = *exp_str + "\tmul r1, r2, r1\n";
  } else { // Neither are int pos int literals
    e1->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpush {r1}\n";
    e2->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\tpop {r2}\n\tmul r1, r2, r1\n";
  }
}

void Not::assem(string * exp_str, string * branchLabel) {
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
    expr->e->assem(exp_str, branchLabel);
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
      expr->e->assem(exp_str, branchLabel);
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
        nExp->assem(exp_str, branchLabel);
        *exp_str = *exp_str + "\teor r1, r1, #1\n";
      } else {
        string label("not" + to_string(expCnt++));
        nExp->assem(exp_str, &label);
        *exp_str = *exp_str + "\tb " + *branchLabel + "\n"+ label + ":\n";
      }
    }
  } else {
    if (!branchLabel) {
      e->assem(exp_str, nullptr);
      *exp_str = *exp_str + "\teor r1, r1, #1\n";
    } else {
      string label("not" + to_string(expCnt++));
      e->assem(exp_str, &label);
      *exp_str = *exp_str + "\tb " + *branchLabel + "\n"+ label + ":\n";
    }
  }
}

void Pos::assem(string * exp_str, string * branchLabel) {
  e->assem(exp_str, nullptr);
}

void Neg::assem(string * exp_str, string * branchLabel) {
  if (dynamic_cast<LitInt *>( e )) {
    LitInt * expr = (LitInt *) e;
    *exp_str = *exp_str + "\tmov r1, #-"+ to_string(expr->i->i) + "\n";
  } else if (dynamic_cast<Neg *>( e )) {
    Neg * expr = (Neg *) e;
    expr->e->assem(exp_str, nullptr);
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
      expr->e->assem(exp_str, nullptr);
    } else { // something else
      nExp->assem(exp_str, nullptr);
      *exp_str = *exp_str + "\trsb r1, r1, #0\n";
    }
  } else {
    e->assem(exp_str, nullptr);
    *exp_str = *exp_str + "\trsb r1, r1, #0\n";
    //*exp_str = *exp_str + "\tmov r2, #-1\n\tmul r1, r1, r2\n";
  }
}

void ParenExp::assem(string * exp_str, string * branchLabel) {
  e->assem(exp_str, branchLabel);
}

void LitInt::assem(string * exp_str, string * branchLabel) {
  *exp_str = *exp_str + "\tmov r1, #"+ to_string(i->i) + "\n";
}

void True::assem(string * exp_str, string * branchLabel){
  // Do Nothing - true will always execute the first part of the statement
  if (branchLabel == nullptr){
    *exp_str = *exp_str + "\tmov r1, #1\n";
  }
}

void False::assem(string * exp_str, string * branchLabel){
  // False always branches
  if (branchLabel == nullptr){
    *exp_str = *exp_str + "\tmov r1, #0\n";
  } else {
    *exp_str = *exp_str + "\tb " + *branchLabel + "\n";
  }
}

void ExpObject::assem(string * exp_str, string * branchLabel) {
  o->assem(exp_str, branchLabel);
}

void ObjectMethodCall::assem(string * exp_str, string * branchLabel) {
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

void IdObj::assem(string * exp_str, string * branchLabel){
  map<string, string*> * table = nameTableStack.back();
  if (table->find(*(i->id)) == table->end()
      && nameTableStack.size() > 1) {
    table = nameTableStack[nameTableStack.size() - 2];
  }
  string * label = (*table)[*(i->id)];
  if (label) {
    *exp_str = *exp_str + "\tldr r0, =" + *label + "\n";
    *exp_str = *exp_str + "\tldr r1, [r0]" + "\n";
  }
  //*stmt_str = *stmt_str + "\tstr r1, [r0]" +  "\n";
}

void NewTypeObj::assem(string * exp_str, string * branchLabel) {
  /*
  map<string, string*> * table = nameTableStack.back();
  if (table->find(*(i->id)) == table->end()
      && nameTableStack.size() > 1) {
    //cout << "table working" << endl;
    table = nameTableStack[nameTableStack.size() - 2];
  }
  string * label = (*table)[*(i->id)];
  if (label) {
    *exp_str = *exp_str + "\tldr r0, =" + *label + "\n";
    *exp_str = *exp_str + "\tldr r1, [r0]" + "\n";
  }*/
}

void StringLiteral::assem() {
  programRoot->textSection->push_back(new string("str" + to_string(strCnt) + ": .asciz \"" + str->c_str() + "\""));
  label = new string("str" + to_string(strCnt++));
}

#endif
