#include "tree.h"
#include <fstream>
#include <vector>
#include <cassert>

using namespace std;

TreeNode::TreeNode() {
	father = NULL;
	anc = left = right = 0;
}

TreeNode::TreeNode(string l) {
	label = l;
	llabel = hashFunc(label);
	father = NULL;
	anc = left = right = 0;
}

TreeNode::~TreeNode() {
}

TreeNode *TreeNode::read(ifstream &fin, int &no) {
	string tag;
	getline(fin, tag);
	//cout << no++ << "  " << tag << endl;
	TreeNode *ret = new TreeNode(tag);
	int n;
	fin >> n;
	getline(fin, tag);
	//cout << no++ << "  " << n << endl;
	for (int i = 0; i < n; ++i)
		ret->insertChild(read(fin, no));
	for (auto & i : ret->child)
		i->father = ret;
	return ret;
}

void TreeNode::readFile(char *filename, int &no) {
	ifstream fin(filename);
	string tag;
	getline(fin, tag);
	//cout << no++ << "  " << tag << endl;
	label = tag;
	int n;
	fin >> n;
	getline(fin, tag);
	//cout << no++ << "  " << n << endl;
	for (int i = 0; i < n; ++i) {
		insertChild(read(fin, no));
	}
	for (auto & i : child)
		i->father = this;
}

bool TreeNode::insertChild(TreeNode *c) {
	child.push_back(c);
	return true;
}

TreeNode *TreeNode::deleteRightmostTree() {
	if (child.empty())
		return NULL;
	TreeNode *ret = child.back();
	child.pop_back();
	return ret;
}

TreeNode *TreeNode::deleteRightmostChild() {
	TreeNode *rightmostChild = deleteRightmostTree();
	if (rightmostChild == NULL)
		return NULL;
	for (auto & i : rightmostChild->child)
		child.push_back(i);
	return rightmostChild;
}

int TreeNode::getSize() {
	return child.size();
}

void TreeNode::calcEulerString() {
	eulerString = label;
	for (auto & i : child) {
		i->calcEulerString();
		if (i->eulerString != "")
			eulerString += "$" + i->eulerString;
	}
	eulerString += "$" + label;
}

void TreeNode::calcSum() {
	sum = 0;
	for (auto & i : child) {
		i->calcSum();
		sum += i->sum;
	}
	++sum;
}

void TreeNode::calcALR() {
	for (int i = 0; i < int(child.size()); ++i) {
		child[i]->anc = anc + 1;
		if (i > 0)
			child[i]->left += child[i - 1]->left + child[i - 1]->sum;
		else
			child[i]->left = left;
	}
	for (int i = int(child.size()) - 1; i >= 0; --i) {
		if (i + 1 < int(child.size()))
			child[i]->right = child[i + 1]->right + child[i + 1]->sum;
		else
			child[i]->right = right;
		child[i]->calcALR();
	}
}

void TreeNode::calcRank(unsigned int &r) {
	rank = r++;
	for (auto & i : child) {
		i->calcRank(r);
	}
}

void TreeNode::calc() {
	calcSum();
	calcALR();
	unsigned int r = 0;
	calcRank(r);
	calcEulerString();
}


