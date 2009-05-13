/*
 * Tree_node.cpp
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#include "Tree_node.h"

Tree_node::Tree_node(int t): type(t), id(-1), negated(false), parent(NULL) {}

Tree_node::Tree_node(int t, int v): type(t), id(v), negated(false), parent(NULL) {}

Tree_node::Tree_node(Tree_node* n){
	type = n->get_type();
	id = n->get_id();
	negated = n->get_negated();

	for(int i=0; i<n->children.size(); ++i)
		children.push_back(new Tree_node(n->children[i]));
}

Tree_node::~Tree_node() {}

void Tree_node::reset(int t, int i){
	type = t;
	id = i;
}
int Tree_node::get_type(){ return type; }
int Tree_node::get_id(){ return id; }
bool Tree_node::get_negated(){ return negated; }

void Tree_node::negate(){
	negated = !negated;
}

void Tree_node::dmg_flip(){
	switch(id){
	case OR:
		id = AND;
		return;
	case AND:
		id = OR;
		return;
	case FOR_ALL:
		id = EXISTS;
		return;
	case EXISTS:
		id = FOR_ALL;
		return;
	default:
		;//maybe an error? meh...
	}
}

bool Tree_node::precedes(int other_id){
	if((type & (BINOP+PBINOP)) == 0)
		throw("Unexpected call to precedes for Tree_node of type "+to_string(type));
	return id < other_id;
}

bool Tree_node::can_follow(int kid_type, int kid_id){
	switch(type){
	case LPAREN:
	case QUANT:
		return  true;
	case VAR:
	case KONST:
		return false;
	case PRED:
	case FUNC:
		if((kid_type & (VAR+KONST+FUNC)) > 0)
			return true;
		return  false;
	case NOT:
		if((kid_type & (BINOP+PBINOP)) > 0)
			return false;
		return true;
	case BINOP:
	case PBINOP:
		if((kid_type & (BINOP+PBINOP)) > 0)
			return !precedes(kid_id);
		return  true;
	default:
		throw("Unexpected call to can_follow for Tree_node of type "+to_string(type));

	}
}

void Tree_node::subs(map<int, Tree_node*> pairs){
	for(int i=0; i<children.size(); ++i){
		if(pairs.find(children[i]->get_id()) != pairs.end()){
			Tree_node* new_kid = new Tree_node(pairs[children[i]->get_id()]);
			children[i] = new_kid;
			new_kid->parent = this;
		}
		children[i]->subs(pairs);
	}
}

bool Tree_node::contains(int i){
	if(id==i)
		return true;
	for(int j=0; j<children.size();  ++j)
		if(children[j]->contains(i))
			return true;
	return false;
}

void Tree_node::print(string tab){
	cout<<tab<<"("<<type<<","<<id<<","<<negated<<")"<<endl;
	for(int i=0; i<children.size(); ++i)
		children[i]->print(tab+"  ");
}
