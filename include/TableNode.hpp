
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

class TreeNode;
class VarDecl;
class MethodDecl;

class Data {
	public:
		std::string * key = nullptr;
		std::string * symbolType = nullptr;
		std::string * type = nullptr; // int, bool, id
};

class SimpleVar : public Data {
	public:
		TreeNode * val = nullptr;

		SimpleVar (std::string * k, const char * t, TreeNode * v) {
			key = k;
			symbolType = new std::string("SimpleVar");
			type = new std::string(t);
			val = v;
		}
};

class RefVar : public Data {
	public:
		//std::string * className = nullptr;
		TreeNode * val = nullptr; // this should b pointer to obj

		RefVar (std::string * k, std::string * c, TreeNode * v) {
			key = k;
			type = c;
			symbolType = new std::string("RefVar");
			val = v;
		}
};

/*class ArrayVar : public Data {
	public:
		int dimCnt = 0;
		TreeNode * val = nullptr; // this should b pointer to the array
		std::string * elKind = nullptr;
		std::string * elType = nullptr;

		ArrayVar (const char * k, int d, TreeNode * v, 
				const char * knd, const char * typ) {
			key = new std::string(k);
			symbolType = new std::string("ArrVar");
			dimCnt = d;
			val = v;
			elKind = new std::string(knd);
			elType = new std::string(typ);
		}
};*/

class ClassVar : public Data {
	public:
		ClassVar * super = nullptr;
		ClassVar (const char * k, ClassVar * super ) {
			key = new std::string(k);
			this->super = super;
			symbolType = new std::string("ClassVar");
		}
};

class MethodVar : public Data {
	public:
		MethodVar (std::string * k) {
			key = k;
			symbolType = new std::string("MethodVar");
		}
};

class FormalParemeterVar : public Data {
	public:
		FormalParemeterVar (const char * k) {
			key = new std::string(k);
			symbolType = new std::string("FormalParemeterVar");
		}
};

class TableNode {
	public:
		TableNode * parent = nullptr;
		std::map<std::string, Data * > * table = nullptr;
		std::vector<VarDecl *> * redecVarDecl = nullptr;
		//std::vector<FormalRest *> * redecVarDecl = nullptr;
		std::vector<MethodDecl *> * redecMethodDecl = nullptr;
		TableNode (TableNode * p) {
			this->parent = p;
			this->table = new std::map<std::string, Data *>;
			redecVarDecl = new std::vector<VarDecl *>;
			redecMethodDecl = new std::vector<MethodDecl *>;
		}
		void appendBadVarDecl(VarDecl * v) {
			redecVarDecl->push_back(v);
		}
		void reportBadVarDecl();
		void appendBadMethodDecl(MethodDecl * m) {
			redecMethodDecl->push_back(m);
		}
		void reportBadMethodDecl();
};

#endif
