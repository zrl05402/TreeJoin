#ifndef TSTRING_H
#define TSTRING_H

#include <vector>
#include <iostream>
#include "hashFunc.h"

using namespace std;


class tString {
public:
	vector<string> m_str;
	vector<int> m_hstr;

	tString() {
	}

	tString(string s) {
		int i = 0;
		int length = s.length();
		while (i < length) {
			int j = i;
			for (; j < length; ++j)
				if (s[j] == '$')
					break;
			m_str.push_back(s.substr(i, j - i));
			i = j + 1;
		}
		for (auto & j : m_str)
			m_hstr.push_back(hashFunc(j));
	}

	~tString() {
	}

	int length() {
		return m_str.size();
	}

	string substr(int pos, int length) {
		string ret = "";
		for (int i = 0; i < length; ++i)
			ret += m_str[pos + i];
		return ret;
	}

	string ssubstr(int pos, int length) {
		string ret = m_str[pos];
		for (int i = 1; i < length; ++i)
			ret += '$' + m_str[pos + i];
		return ret;
	}

	int getElement(int index) {
		return m_hstr[index];
	}
};

#endif