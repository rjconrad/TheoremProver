/*
 * util.cpp
 *
 *  Created on: May 12, 2009
 *      Author: rick
 */
#include "util.h"

string to_string(int i){
	stringstream s;
	s << i;
	return s.str();
}

bool is_letter(char c){
	return ((c>=65 && c<=90) || (c>=97 && c<=122));
}

