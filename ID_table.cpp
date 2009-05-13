/*
 * ID_table.cpp
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#include "ID_table.h"

ID_table::ID_table(): index(100) {}

ID_table::~ID_table() {}

int ID_table::get_id(string handle, int& type){
	if(table.find(handle) != table.end()){
		int id = table[handle];
		if((types[id-100]&type) > 0){
			type = types[id-100];  //some types come in ambiguous (var + konst)
			return id;
		}
		throw "Type mismatch!";
	}

	table[handle]=index;
	++index;
	if(type==Tree_node::VAR + Tree_node::KONST)
		type = Tree_node::KONST;  //a var would have been first used unambiguously
	types.push_back(type);
	arg_counts.push_back(-1);
	return table[handle];
}

bool ID_table::check_arg_count(int id, int arg_count){
	int count = arg_counts.at(id-100);
	if(count==-1){
		arg_counts[id-100] = arg_count;
		return true;
	}
	if(count==arg_count)
		return true;

	throw "Argument Mismatch!";
}

int ID_table::skolemize(int id, int argcount){
	int index = id-100;
	if(types[index] != Tree_node::VAR)
		throw "ID Table cannot skolemize token of type "+to_string(types[index]);
	if(argcount==0){
		types[index] = Tree_node::KONST;
		return Tree_node::KONST;
	}
	else{
		types[index] = Tree_node::FUNC;
		arg_counts[index] = argcount;
		return Tree_node::FUNC;
	}
}

vector<int> ID_table::check_skolem(int id){
	int index = id-100;
	vector<int> type_args;
	type_args.push_back(types[index]);
	type_args.push_back(arg_counts[index]);
	return type_args;
}
