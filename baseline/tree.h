#ifndef TREE_H
#define TREE_H

#include <vector>
#include <iostream>
#include <fstream>
#include "tString.h"

using namespace std;

class TreeNode {
public:	
	string label, eulerString;
	tString postString;
	TreeNode *father;
	vector<TreeNode*> child;
	int anc, left, right, sum;
	unsigned int llabel, rank;

	TreeNode();
	TreeNode(string l);
	~TreeNode();
	TreeNode *read(ifstream &fin, int &no);
	void readFile(char *filename, int &no);
	bool insertChild(TreeNode *c);
	TreeNode *deleteRightmostTree();
	TreeNode *deleteRightmostChild();
	int getSize();
	void calcSum();
	void calcEulerString();
	void calcALR();
	void calcRank(unsigned int &r);
	void calc();
};

#endif