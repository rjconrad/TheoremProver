/*
 * Knowledge.cpp
 *
 *  Created on: May 13, 2009
 *      Author: rick
 */

#include "Knowledge.h"

Knowledge::Knowledge() {
	// TODO Auto-generated constructor stub

}

Knowledge::~Knowledge() {
	// TODO Auto-generated destructor stub
}

int Knowledge::size(){
	return facts.size();
}

void Knowledge::add(Tree_node* node){
	if(node->get_id()!=Tree_node::AND)
		facts.push_back(node);
	else
		for(int i=0; i<node->children.size(); ++i)
			facts.push_back(node->children[i]);
}

bool Knowledge::unify_var(Tree_node* left, Tree_node* right, map<int, Tree_node*>& pairs){
	if(left->get_id()==right->get_id())
		return true;
	if(pairs.find(left->get_type()) != pairs.end())
		return unify(pairs[left->get_id()], right, pairs);
	if(pairs.find(right->get_type()) != pairs.end())
		return unify(left, pairs[right->get_type()], pairs);
	if(!right->contains(left->get_id())){
		pairs[left->get_id()] = right;
		return true;
	}
	return false;
}

bool Knowledge::unify(Tree_node* left, Tree_node* right, map<int,Tree_node*>& pairs){
	if(	left->get_type()==Tree_node::PRED && right->get_type()==Tree_node::PRED ||
		left->get_type()==Tree_node::FUNC && right->get_type()==Tree_node::FUNC){
		if(left->get_id()==right->get_id()){
			for(int i=0; i<left->children.size(); ++i)
				if(!unify(left->children[i], right->children[i], pairs))
					return false;
			cout<<"Unified folks with id: "<<left->get_id()<<" and id: "<<right->get_id()<<endl;
			return true;
		}
		return false;
	}
	if(left->get_id() == right->get_id())
		return true;
	if(left->get_type()==Tree_node::VAR && right->get_type()!=Tree_node::PRED)
		return unify_var(left, right, pairs);
	if(right->get_type()==Tree_node::VAR && left->get_type()!=Tree_node::PRED)
		return unify_var(right, left, pairs);
}

void Knowledge::find_unifies(Tree_node* node){
	for(int i=0;  i<facts.size(); ++i){
		if(facts[i]->get_id()==Tree_node::OR){
			for(int j=0; j<facts[i]->children.size(); ++j){
				map<int,Tree_node*> pairs;
				if(node!=facts[i]->children[j] && unify(node, facts[i]->children[j], pairs)){
					cout<<"Unified--------------------------"<<endl;
					node->print("  ");
					facts[i]->children[j]->print("  ");
					cout<<"----------------------------------";
				}

			}
		}
		map<int,Tree_node*> pairs;
		if(node!=facts[i] && unify(node, facts[i], pairs)){
			cout<<"Unified--------------------------"<<endl;
			node->print("  ");
			facts[i]->print("  ");
			cout<<"----------------------------------";
		}
	}
}

void Knowledge::find_unifies(){
	for(int i=0;  i<facts.size(); ++i){
		if(facts[i]->get_id()==Tree_node::OR)
			for(int j=0; j<facts[i]->children.size(); ++j)
				find_unifies(facts[i]->children[j]);
		find_unifies(facts[i]);
	}
}

vector<Tree_node*> Knowledge::get_preds(vector<Tree_node*>::iterator f){
	if((*f)->get_id()==Tree_node::OR)
		return (*f)->children;
	vector<Tree_node*> preds;
	preds.push_back(*f);
	return preds;
}

bool Knowledge::resolve(vector<Tree_node*>::iterator f,vector<Tree_node*>::iterator g,
						vector<Tree_node*> f_preds, vector<Tree_node*> g_preds,
						vector<Tree_node*>::iterator h, vector<Tree_node*>::iterator i){
	if((*h)->get_negated() ^ (*i)->get_negated()){
		map<int, Tree_node*> pairs;
		if(unify(*h, *i, pairs)){
			//f_preds.erase(h);
			//g_preds.erase(i);
			int num = f_preds.size()+g_preds.size()-2;
			if(num > 0){
				Tree_node* new_fact;
				if(num>1)
					new_fact = new Tree_node(Tree_node::BINOP, Tree_node::OR);
				for(vector<Tree_node*>::iterator x=f_preds.begin(); x!=f_preds.end(); ++x){
					if((*x)!=(*h)){
						if(num>1)
							new_fact->children.push_back(*x);
						else
							new_fact = *x;
					}
				}
				for(vector<Tree_node*>::iterator x=g_preds.begin(); x!=g_preds.end(); ++x){
					if((*x)!=(*i)){
						if(num>1)
							new_fact->children.push_back(*x);
						else
							new_fact = *x;
					}
				}
				new_fact->subs(pairs);
				facts.push_back(new_fact);
			}
			facts.erase(g);
			facts.erase(f);
			cout<<"resolved"<<endl;
			print();
			return true;
		}
	}
	return false;
}

bool Knowledge::resolution(){
	for(vector<Tree_node*>::iterator f=facts.begin(); f!=facts.end(); ++f){
		for(vector<Tree_node*>::iterator g=(f+1); g!=facts.end(); ++g){
			if(f!=g){
				vector<Tree_node*> f_preds = get_preds(f);
				vector<Tree_node*> g_preds = get_preds(g);
				for(vector<Tree_node*>::iterator h=f_preds.begin(); h!=f_preds.end(); ++h)
					for(vector<Tree_node*>::iterator i=g_preds.begin(); i!=g_preds.end(); ++i)
						if(resolve(f,g,f_preds,g_preds,h,i)){
							return true;
						}
			}
		}
	}
	return false;
}

void Knowledge::print(){
	for(int i=0; i<facts.size(); ++i){
		cout<<"Fact "<<i<<":..."<<endl;
		facts[i]->print("  ");
	}
}
