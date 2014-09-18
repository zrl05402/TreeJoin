#include "tString.h"

using namespace std;

tString::tString() {
}

tString::tString(string s) {
	int i = 0;
	int length = s.length();
	m_str.clear();
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

tString::~tString() {
}

int tString::length() {
	return m_str.size();
}

string tString::substr(int pos, int length) {
	string ret = "";
	for (int i = 0; i < length; ++i)
		ret += m_str[pos + i];
	return ret;
}

string tString::ssubstr(int pos, int length) {
	string ret = m_str[pos];
	for (int i = 1; i < length; ++i)
		ret += '$' + m_str[pos + i];
	return ret;
}

int tString::getElement(int index) {
	return m_hstr[index];
}

string &tString::getString(int index) {
	return m_str[index];
}