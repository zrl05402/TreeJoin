#ifndef HASHFUNC_H
#define HASHFUNC_H

#include <iostream>

using namespace std;

unsigned int hashFunc(string &s) {
	unsigned int seed = 131;
	unsigned int hash = 0;
	for (int i = 0; i < int(s.length()); ++i)
		hash = hash * seed + s[i];
	return (hash & 0x7FFFFFFF);
}

#endif