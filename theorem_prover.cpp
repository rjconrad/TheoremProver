/*
 * theorem_prover.cpp
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#include "common.h"
#include "Token_stream.h"
#include "Tree_node.h"
#include "ID_table.h"
#include "Knowledge.h"

const int VAR = Tree_node::VAR;
const int NEW_EXPR = Tree_node::PRED+Tree_node::LPAREN+Tree_node::QUANT+Tree_node::NOT;
const int END_EXPR = Tree_node::BINOP+Tree_node::NOTHING;
const int ARG_PUNC = Tree_node::RPAREN+Tree_node::COMMA;
const int ARG = Tree_node::FUNC+Tree_node::KONST+Tree_node::VAR;
const int END_PAREN = Tree_node::RPAREN+Tree_node::PBINOP;
const int QUANT_BODY = NEW_EXPR + Tree_node::QCOMMA;

void add_expectations(stack<int>& expects, int type){
	switch(type){
	case Tree_node::LPAREN:
		expects.push(END_PAREN);
		expects.push(NEW_EXPR);
		return;
	case Tree_node::PRED:
		expects.push(ARG_PUNC);
		expects.push(ARG);
		return;
	case Tree_node::QUANT:
		expects.push(QUANT_BODY);
		expects.push(VAR);
		return;
	case Tree_node::NOT:
		expects.push(NEW_EXPR);
		return;
	case Tree_node::BINOP:
		expects.push(END_EXPR);
		expects.push(NEW_EXPR);
		return;
	case Tree_node::COMMA:
		expects.push(ARG_PUNC);
		expects.push(ARG);
		return;
	case Tree_node::QCOMMA:
		expects.push(QUANT_BODY);
		expects.push(VAR);
		return;
	case Tree_node::FUNC:
		expects.push(ARG_PUNC);
		expects.push(ARG);
		return;
	case Tree_node::PBINOP:
		expects.push(END_PAREN);
		expects.push(NEW_EXPR);
		return;
	default:
		//rparen, var, konst do not add anything to stack
		return;
	}
}

Tree_node* add_node(Tree_node*& root, Tree_node* last, Tree_node* current){
	//Do nothing for commas
	if((current->get_type() & (Tree_node::COMMA+Tree_node::QCOMMA)) > 0)
		return last;

	//rparens let last to be the parent of their opening lparen
	if(current->get_type()==Tree_node::RPAREN){
		const int PAREN_OPENERS = Tree_node::LPAREN+Tree_node::FUNC+Tree_node::PRED;
		while((last->get_type() & PAREN_OPENERS)==0)
			last = last->parent;
		last = last->parent;
		return last;
	}

	//find a node that can be this one's parent
	while(last!=NULL && !last->can_follow(current->get_type(), current->get_id()))
		last = last->parent;

	//we passed the root
	if(last==NULL){
		current->children.push_back(root);
		root = current;
		last = current;
		return last;
	}

	//BINOPs intercept last  child of parent
	if((current->get_type() & (Tree_node::BINOP+Tree_node::PBINOP)) > 1){
		Tree_node* grandkid = last->children.back();
		last->children.pop_back();
		last->children.push_back(current);
		current->parent = last;
		current->children.push_back(grandkid);
		grandkid->parent = current;
		last = current;
		return last;
	}

	//easy peasy...add to  parent
	last->children.push_back(current);
	current->parent = last;
	last = current;
	return last;
}

Tree_node* make_tree(Token_stream& ts, ID_table& id_table){
	stack<int> expects;
	expects.push(END_EXPR);
	Tree_node* last = NULL;
	Tree_node* current = NULL;
	Tree_node* root = NULL;

	root = ts.get_tree_node(id_table, NEW_EXPR);
	add_expectations(expects, root->get_type());
	last = root;
	while(true){
		int expect = expects.top();
		expects.pop();
		current = ts.get_tree_node(id_table, expect);
		if(current->get_type() == Tree_node::NOTHING)
			return root;
		add_expectations(expects, current->get_type());
		last = add_node(root, last, current);  //root is passed byref
	}
}

void elim_paren(Tree_node* node, Tree_node*& root){
	if((node->get_type() & (Tree_node::LPAREN+Tree_node::NOT)) > 0){
		if(node==root){
			root = node->children[0];
			root->parent = NULL;
		}
		else{
			vector<Tree_node*>* siblings = &(node->parent->children);
			for(int i=0; i<siblings->size(); ++i)
				if((*siblings)[i]==node)
					(*siblings)[i]=node->children[0]; //num of children of ( must be exactly 1
			node->children[0]->parent = node->parent;
		}
		if(	node->get_type()==Tree_node::NOT && !node->get_negated() ||
			node->get_type()!=Tree_node::NOT && node->get_negated())
			node->children[0]->negate();
	}
	for(int i=0; i<node->children.size(); ++i)
		elim_paren(node->children[i], root);
}

void elim_imp(Tree_node* node){
	if(node->get_id()==Tree_node::IMP){
		node->reset(Tree_node::BINOP, Tree_node::OR);
		node->children[0]->negate();
	}
	if(node->get_id()==Tree_node::DBL_IMP){
		node->reset(Tree_node::BINOP, Tree_node::AND);
		Tree_node* left = node->children[0];
		Tree_node* right = node->children[1];
		Tree_node* new_left = new Tree_node(left);
		Tree_node* new_right = new Tree_node(right);

		//negations
		left->negate();
		new_right->negate();

		//insert on left side
		node->children[0] = new Tree_node(Tree_node::BINOP, Tree_node::OR);
		node->children[0]->children.push_back(left);
		left->parent = node->children[0];
		node->children[0]->children.push_back(right);
		right->parent = node->children[0];

		//insert on right side
		node->children[1] = new Tree_node(Tree_node::BINOP, Tree_node::OR);
		node->children[1]->children.push_back(new_left);
		new_left->parent = node->children[1];
		node->children[1]->children.push_back(new_right);
		new_right->parent = node->children[1];
	}
	for(int i=0; i<node->children.size(); ++i)
		elim_imp(node->children[i]);
}

void demorgan(Tree_node* node, bool neg){
	switch(node->get_type()){
	case Tree_node::PRED:
		if(neg)
			node->negate();
		return;  //no children of a pred will need this op

	case Tree_node::BINOP:
	case Tree_node::PBINOP:
	case Tree_node::QUANT:
		if(neg ^ node->get_negated())
			node->dmg_flip();
		if(node->get_negated()){
			node->negate();
			neg = !neg;
		}
		break;
	}

	for(int i=0; i<node->children.size(); ++i)
		demorgan(node->children[i], neg);
}

void skolemize(Tree_node* node, vector<int> vars, ID_table& id_table){
	switch(node->get_type()){
	case Tree_node::QUANT:
		if(node->get_id()==Tree_node::FOR_ALL){
			for(int i=0; i<(node->children.size()-1); ++i)
				vars.push_back(node->children[i]->get_id());
		}
		else{  //EXISTS
			for(int i=0; i<(node->children.size()-1); ++i)
				id_table.skolemize(node->children[i]->get_id(), vars.size());
		}
		skolemize(node->children.back(), vars, id_table);
		return;
	case Tree_node::VAR:
		vector<int> type_args = id_table.check_skolem(node->get_id()); //type_args is array len 2
		if(type_args[0]!=Tree_node::VAR){
			node->reset(type_args[0], node->get_id());
			for(int i=0; i<type_args[1]; ++i)  //get the first arg_count vars
				node->children.push_back(new Tree_node(Tree_node::VAR, vars[i]));
		}
		return;
	}
	for(int i=0; i<node->children.size(); ++i)
		skolemize(node->children[i], vars, id_table);
}

void elim_quant(Tree_node* node, Tree_node*& root){
	if(node->get_type()==Tree_node::QUANT){
		if(node==root){
			root = node->children.back();
			root->parent = NULL;
			elim_quant(root, root);
			return;
		}
		vector<Tree_node*>* siblings = &(node->parent->children);
		for(int i=0; i<siblings->size(); ++i)
			if((*siblings)[i]==node)
				(*siblings)[i]=node->children.back();
		node->children.back()->parent = node->parent;
		elim_quant(node->children.back(), root);
		return;
	}
	for(int i=0; i<node->children.size(); ++i)
		elim_quant(node->children[i], root);
}

void flatten(Tree_node* node, Tree_node*& root){
	if((node->get_type()&(Tree_node::BINOP+Tree_node::PBINOP))>0){
		if(node!=root){
			if(node->get_id()==node->parent->get_id()){
				for(int i=0; i<node->parent->children.size(); ++i){
					if(node->parent->children[i]!=node){
						node->children.push_back(node->parent->children[i]);
						node->children.back()->parent = node;
					}
				}
				Tree_node* old_p = node->parent;
				node->parent = node->parent->parent;
				if(node->parent==NULL)
					root = node;
				else
					for(int i=0; i<node->parent->children.size(); ++i)
						if(old_p = node->parent->children[i])
							node->parent->children[i] = node;
			}

			if(node->get_id()==Tree_node::AND){  //and cannot be a child node
				for(int i=0; i<node->children.size(); ++i){
					Tree_node* kid = node->children[i];
					node->children[i] = new Tree_node(Tree_node::BINOP, Tree_node::OR);
					node->children[i]->parent = node;
					node->children[i]->children.push_back(kid);
					kid->parent = node->children[i];
					for(int j=0; j<node->parent->children.size(); ++j){
						if(node->parent->children[j]!=node){
							Tree_node* copy = new Tree_node(node->parent->children[j]);
							node->children[i]->children.push_back(copy);
							copy->parent = node->children[i];
						}
					}
				}
				Tree_node* old_p = node->parent;
				node->parent = node->parent->parent;
				if(node->parent==NULL)
					root = node;
				else
					for(int i=0; i<node->parent->children.size(); ++i)
						if(old_p = node->parent->children[i])
							node->parent->children[i] = node;
				flatten(node, root);
				return;
			}
		}
		for(int i=0; i<node->children.size(); ++i)
			flatten(node->children[i], root);
	}
}

void process(Tree_node*& root, ID_table& id_table){
	elim_paren(root, root);  //2nd root passed byref
	elim_imp(root);
	demorgan(root, false);
	skolemize(root, vector<int>(), id_table);
	elim_quant(root, root);  //2nd root passed byref
	flatten(root, root);    //2nd root passed byref
}

int main(){
	Knowledge knowledge;
	ID_table id_table;
	while(true){
		string s;
		//cin>>s;
		getline(cin, s);
		if(s.compare("")==0)
			break;
		Token_stream ts(s);
		try{
			Tree_node* root = make_tree(ts, id_table);
			process(root, id_table);
			//root->print("");
			knowledge.add(root);
		}
		catch(char const* e){
			cout<<"Error: "<<e;
			return 1;
		}
		catch(string e){
			cout<<"Error: "<<e;
			return 1;
		}
	}
	knowledge.print();
	while(knowledge.resolution());
	if(knowledge.size()==0)
		cout<<"true"<<endl<<endl;
	else
		cout<<"false"<<endl<<endl;
	//knowledge.print();
	return 0;
}
