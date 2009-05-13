/*
 * Knowledge.h
 *
 *  Created on: May 13, 2009
 *      Author: rick
 */

#ifndef KNOWLEDGE_H_
#define KNOWLEDGE_H_

#include "common.h";
#include "Tree_node.h";

class Knowledge {
public:
	Knowledge();
	virtual ~Knowledge();

	int size();
	void add(Tree_node*);
	bool unify_var(Tree_node*, Tree_node*, map<int, Tree_node*>&);
	bool unify(Tree_node*, Tree_node*, map<int,Tree_node*>&);
	void find_unifies(Tree_node*);
	void find_unifies();
	vector<Tree_node*> get_preds(vector<Tree_node*>::iterator);
	bool resolve(	vector<Tree_node*>::iterator,vector<Tree_node*>::iterator,
					vector<Tree_node*>, vector<Tree_node*>,
					vector<Tree_node*>::iterator, vector<Tree_node*>::iterator);
	bool resolution();
	void print();

private:
	vector<Tree_node*> facts;
};

#endif /* KNOWLEDGE_H_ */
