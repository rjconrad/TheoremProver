/*
 * ID_table.h
 *
 *  Created on: Mar 24, 2009
 *      Author: rick
 */

#ifndef ID_TABLE_H_
#define ID_TABLE_H_

#include "common.h"
#include "Tree_node.h"

class ID_table {
public:
	ID_table();
	virtual ~ID_table();

	int get_id(string, int&);
	bool check_arg_count(int, int);
	int skolemize(int, int);
	vector<int> check_skolem(int);

private:
	int index;
	map<string, int> table;  //string is user-def handle, int is our id
	vector<int> types; //index is id, val it type
	vector<int> arg_counts;  //index is id, val is arg_count
};

#endif /* ID_TABLE_H_ */
