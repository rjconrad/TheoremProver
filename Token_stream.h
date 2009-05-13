/*
 * Token_stream.h
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#ifndef TOKEN_STREAM_H_
#define TOKEN_STREAM_H_

#include "common.h"
#include "util.h"
#include "Tree_node.h"
#include "ID_table.h"

class Token_stream {
public:
	Token_stream(string);
	virtual ~Token_stream();

	int get_token(int&, string&);
	Tree_node* get_tree_node(ID_table&, int);

private:
	stringstream input;

	//bool is_letter(char);
};

#endif /* TOKEN_STREAM_H_ */
