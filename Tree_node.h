/*
 * Tree_node.h
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#ifndef TREE_NODE_H_
#define TREE_NODE_H_

#include "common.h"
#include "util.h"

class Tree_node {
public:
	Tree_node(int);
	Tree_node(int,int);
	Tree_node(Tree_node*);
	virtual ~Tree_node();

	void reset(int, int);
	int get_type();
	int get_id();
	bool get_negated();
	void negate();
	void dmg_flip();
	bool precedes(int);
	bool can_follow(int, int);
	bool contains(int);
	void subs(map<int,Tree_node*>);
	void print(string);

	//types .. values chosen to enable useful bitwise operations
	static const int NOTHING = 1;
	static const int LPAREN = 2;
	static const int RPAREN = 4;
	static const int PRED = 8;
	static const int QUANT = 16;
	static const int NOT = 32;
	static const int BINOP = 64;
	static const int COMMA = 128;
	static const int QCOMMA = 256;
	static const int KONST = 512;
	static const int VAR = 1024;
	static const int FUNC = 2048;
	static const int PBINOP = 4096;

	//ids for binops and quants
	//values are significant for evaluating operator precedence
	//don't add to them w/o changing initial index in ID_table
	static const int AND = 1;
	static const int OR = 2;
	static const int IMP = 4;
	static const int DBL_IMP = 8;
	static const int FOR_ALL = 16;
	static const int EXISTS = 32;

	vector<Tree_node*> children;
	Tree_node* parent;
private:
	int type;
	//int value;
	int  id;  //for var, konst, pred, func, binop, quant
	//vector<int> vars; //for quants
	bool negated;

};

#endif /* TREE_NODE_H_ */
