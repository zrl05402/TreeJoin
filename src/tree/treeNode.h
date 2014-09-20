#ifndef TREE_H
#define TREE_H

#include "treeString.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

using namespace std;

class TreeNode {
public:	
	string label;
	TreeString postOrderedString;
	TreeNode *father;
	vector<TreeNode*> children;
	vector<string> eulerString;
	int sum;
	unsigned int hlabel, rank;

	TreeNode() {
		father = NULL;
	}

	TreeNode(string l) {
		label = l;
		hlabel = hashFunc(label);
		father = NULL;
	}

	~TreeNode() {
	}

	bool insertChild(TreeNode *c) {
		children.push_back(c);
		return true;
	}

	TreeNode *deleteRightmostTree() {
		if (children.empty())
			return NULL;
		TreeNode *ret = children.back();
		children.pop_back();
		return ret;
	}

	TreeNode *deleteRightmostChild() {
		TreeNode *rightmostChild = deleteRightmostTree();
		if (rightmostChild == NULL)
			return NULL;
		for (auto & i : rightmostChild->children)
			children.push_back(i);
		return rightmostChild;
	}

	int size() {
		return children.size();
	}

	void calcSum() {
		sum = 0;
		for (auto & i : children) {
			i->calcSum();
			sum += i->sum;
		}
		++sum;
	}

	void calcEulerString() {
		eulerString.push_back(label);
		int step = 0;
		for (auto & i : children) {
			i->calcEulerString();
			step = max(step, int(i->eulerString.size()));
		}
		for (int i = 0; i < step; ++i) {
			string tmp = label;
			for (auto & j : children) 
				if (int(j->eulerString.size()) > i)
					tmp += "$" + j->eulerString[i];
				else 
					tmp += "$" + j->eulerString[j->eulerString.size() - 1];
			eulerString.push_back(tmp);
		}
	}

	void calcRank(unsigned int &r) {
		rank = r++;
		for (auto & i : children) {
			i->calcRank(r);
		}
	}

	void calcPostOrderedString() {
		string ret = "";
		for (auto & i : children) {
			i->calcPostOrderedString();
			ret.append(i->postOrderedString.get() + "$");
		}
		ret.append(label);
		postOrderedString = TreeString(ret);
	}

	void calc() {
		calcSum();
		unsigned int r = 0;
		calcRank(r);
		calcEulerString();
	}
};

TreeNode *read(ifstream &fin) {
	string tag;
	getline(fin, tag);
	TreeNode *ret = new TreeNode(tag);
	getline(fin, tag);
	int n = atoi(tag.c_str());
	if (n > 0) {
		for (int i = 0; i < n; ++i)
			ret->insertChild(read(fin));
	} else {
		getline(fin, tag);
		ret->insertChild(new TreeNode(tag));
	}
	for (auto & i : ret->children)
		i->father = ret;
	return ret;
}

TreeNode* readFile(char *filename) {
	ifstream fin(filename);
	return read(fin);
}

#endif