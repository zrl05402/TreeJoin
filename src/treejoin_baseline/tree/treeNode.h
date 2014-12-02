#ifndef TREE_H
#define TREE_H

#include "treeString.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class TreeNode {
public:	
	string label, eulerString;
	treeString postOrderedString;
	TreeNode *father;
	vector<TreeNode*> children;
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

	TreeNode *read(ifstream &fin, int &no) {
		string tag;
		getline(fin, tag);
		TreeNode *ret = new TreeNode(tag);
		int n;
		fin >> n;
		getline(fin, tag);
		for (int i = 0; i < n; ++i)
			ret->insertChild(read(fin, no));
		for (auto & i : ret->children)
			i->father = ret;
		return ret;
	}

	void readFile(char *filename, int &no) {
		ifstream fin(filename);
		string tag;
		getline(fin, tag);
		label = tag;
		int n;
		fin >> n;
		getline(fin, tag);
		for (int i = 0; i < n; ++i) {
			insertChild(read(fin, no));
		}
		for (auto & i : children)
			i->father = this;
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
		eulerString = label;
		for (auto & i : children) {
			i->calcEulerString();
			if (i->eulerString != "")
				eulerString += "$" + i->eulerString;
		}
		eulerString += "$" + label;
	}

	void calcRank(unsigned int &r) {
		rank = r++;
		for (auto & i : children) {
			i->calcRank(r);
		}
	}

	void calc() {
		calcSum();
		unsigned int r = 0;
		calcRank(r);
		calcEulerString();
	}
};

#endif