/*
 * Token_stream.cpp
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#include "Token_stream.h"

Token_stream::Token_stream(string s){
	input.str(s);
}
Token_stream::~Token_stream() {}

int Token_stream::get_token(int& value, string& handle){
	char c;
	input>>c;

	if(input.eof())
		return Tree_node::NOTHING;

	switch(c){
	case '!':
		return Tree_node::NOT;
	case '(':
		return Tree_node::LPAREN;
	case ')':
		return Tree_node::RPAREN;
	case ',':
		return Tree_node::COMMA + Tree_node::QCOMMA;
	case '&':
		value = Tree_node::AND;
		return Tree_node::BINOP + Tree_node::PBINOP;
	case '|':
		value = Tree_node::OR;
		return Tree_node::BINOP + Tree_node::PBINOP;
	case '-':
		if(input.peek()!='>')
			throw "Invalid input. Did you mean '->'?";
		input >> c;
		value = Tree_node::IMP;
		return Tree_node::BINOP + Tree_node::PBINOP;
	case '<':
		if(input.peek()!='-' && input.peek()!='>')
			throw "Invalid input. Did you mean '<->'?";
		input >> c >> c;
		value = Tree_node::DBL_IMP;
		return Tree_node::BINOP + Tree_node::PBINOP;
	default:
		if(is_letter(c)){
			string s = string(1,c);
			while(is_letter(input.peek())){
				input>>c;
				s.push_back(c);
			}
			if(s.compare("forall")==0){
				value = Tree_node::FOR_ALL;
				return Tree_node::QUANT;
			}
			if(s.compare("exists")==0){
				value = Tree_node::EXISTS;
				return Tree_node::QUANT;
			}
			if(input.peek()=='('){
				input>>c;  //discard lparen
				handle = s;
				return Tree_node::PRED + Tree_node::FUNC;
			}
			handle = s;
			return Tree_node::VAR + Tree_node::KONST;
		}
		string e = "Inappropriate character in input: ";
		e.push_back(c);
		throw e;
	}
}

Tree_node* Token_stream::get_tree_node(ID_table& id_table, int expect){
	int value = 0;
	string handle = "";
	int token = get_token(value, handle);  //value and handle are passed byref
	int type = token & expect;
	if((type) == 0)
		throw "Unexpected token "+to_string(token);
	int userdef = Tree_node::VAR + Tree_node::KONST + Tree_node::FUNC + Tree_node::PRED;
	if((type & userdef) > 0){
		int id = id_table.get_id(handle, type);  //type is passed byref
		return new Tree_node(type, id);
	}

	int binop_or_quant = Tree_node::BINOP+Tree_node::PBINOP+Tree_node::QUANT;
	if((type & binop_or_quant) > 0)
		return new Tree_node(type, value);

	return new Tree_node(type);
}
