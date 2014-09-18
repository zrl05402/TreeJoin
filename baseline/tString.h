#ifndef TSTRING_H
#define TSTRING_H

#include <vector>
#include <iostream>
#include "hashFunc.h"

using namespace std;


class tString {
private:
	vector<string> m_str;
	vector<int> m_hstr;
public:
	tString();
	tString(string s);
	~tString();
	int length();
	string substr(int pos, int length);
	string ssubstr(int pos, int length);
	int getElement(int index);
	string &getString(int index);
};

#endif