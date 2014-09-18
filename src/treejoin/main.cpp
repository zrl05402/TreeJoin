#include <iostream>
#include "../tree/treeNode.h"
#include "../tree/treeString.h"
#include "../tree/treeED.h"
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cassert>

using namespace std;

unordered_map<string, int> M;

void addToMap(TreeNode *root) {
	M[root->eulerString] += 1;
	for (auto & i : root->children)
		addToMap(i);
}

void addToList(vector<pair<TreeNode*, int> > &list, TreeNode *root) {
	list.push_back(make_pair(root, M[root->eulerString]));
	for (auto & i : root->children)
		addToList(list, i);
}

bool PairCompare(const pair<TreeNode*, int> &a, const pair<TreeNode*, int> &b) {
	return a.second == b.second ? (a.first)->eulerString.length() > (b.first)->eulerString.length() : a.second < b.second;
}

bool ResultCompare(const pair<int, int> &a, const pair<int, int> &b) {
	return a.first == b.first ? a.second < b.second : a.first < b.first;
}

void TreeJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = f.size();
	unordered_map<string, vector<int> > L;
	vector<int> disjoin(n, 0);
	for (int i = 0; i < n; ++i) {
		//get the list
		vector<pair<TreeNode*, int> > list;
		addToList(list, f[i]);
		sort(list.begin(), list.end(), PairCompare);

		//get the prefix
		int m = list.size(), num = 1;
		vector<int> flag(m, 0);
		flag[0] = 1;
		int k;
		for (k = 1; k < m; ++k) {
			if (num == threshold + 1)
				break;
			flag[k] = 1;
			for (int j = 0; j < k; ++j)
				if (list[j].first == (list[k].first)->father) {
					if (flag[j] == 1) {
						flag[j] = 0;
					}
					else {
						++num;
					}
					break;
				}
		}

		//get the candidates
		vector<int> candidates;
		unordered_map<int, bool> isDup;
		if (num < threshold + 1) {
			/*
			for (int j = 0; j <= i; ++j)
				if (disjoin[j] == 0 && abs(int(f[j]->postString.length()) - int(f[i]->postString.length())) < threshold) {
					candidates.push_back(j);
				}
			*/
		}
		else {
			disjoin[i] = 1;
			for (int j = 0; j < k; ++j)
				//if (flag[j] == 1) {
					if (L.find((list[j].first)->eulerString) != L.end()) {
						for (auto & l : L[(list[j].first)->eulerString]) {
							//some pruning techniques
							//PRUNING 1
	 						if (isDup.find(l) == isDup.end()) {
	 							if (abs(int(f[l]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
									candidates.push_back(l);
								}
								isDup[l] = true;
							}
							//PRUNING 2
						}
					}
				//}
			candidates.push_back(i);
		}
		for (auto & j : candidates) {
			result.push_back(make_pair(i, j));
		}

		//verification
		/*
		for (auto & j : candidates) {
			if (treeED(f[i], ff[j]) <= threshold) {
				result.push_back(make_pair(i, j));
				if (i != j)
					result.push_back(make_pair(j, i));
			}
		}
		*/

		//indexing all the prefix
		for (int j = 0; j < k; ++j) {
			if (L.find((list[j].first)->eulerString) == L.end()) {
				vector<int> temp;
				L[(list[j].first)->eulerString] = temp;
			}
			L[(list[j].first)->eulerString].push_back(i);
		}
	}
	//sort(result.begin(), result.end(), ResultCompare);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Usage : " << endl;
		cout << "main filename" << endl;
		return 0;
	}
	cout << "reading..." << endl;
	TreeNode *tree = new TreeNode();
	int no = 1;
	tree->readFile(argv[1], no);
	cout << "reading finished" << endl;

	int totalNum = generatePostOrderedString(tree, "strings.txt");
	cout << "totalNum = " << totalNum << endl;

	vector<TreeNode*> f;
	int totalSum = 0;
	for (int i = 0; i < totalNum; ++i) {
		f.push_back(tree->children[i]);
		tree->children[i]->calc();
		totalSum += tree->children[i]->sum;
		addToMap(f[i]);
	}
	cout << "totalSum = " << totalSum << endl;

	//calcSum(tree);

	
	for (int i = 1; i <= 15; ++i) {
		int edThreshold = i;
		vector<pair<int, int> > result1, result2, result;
		cout << "the threshold = " << i << endl;
		clock_t begin = clock();
		TreeJoin(f, i, result1);
		clock_t end = clock();
		cout << "the result of prefix filter = " << result1.size() << endl;
		cout << "the time of prefix filter = " << (end - begin) / CLOCKS_PER_SEC << endl;
		begin = clock();
		for (auto & j : result1)
			if (getED(f[j.first]->postOrderedString, f[j.second]->postOrderedString, edThreshold) <= edThreshold)
				result2.push_back(make_pair(j.first, j.second));
		end = clock();
		cout << "the result of String ED = " << result2.size() << endl;
		cout << "the time of String ED = " << (end - begin) / CLOCKS_PER_SEC << endl;
		begin = clock();
		for (auto & j : result2)
			if (j.first == j.second || treeED(f[j.first], f[j.second], edThreshold) + costFunc(f[j.first]->hlabel, f[j.second]->hlabel) <= edThreshold)
				result.push_back(make_pair(j.first, j.second));
		end = clock();
		cout << "the number of the answers = " << result.size() << endl;
		cout << "the time of TreeED = " << (end - begin) / CLOCKS_PER_SEC << endl;
		cout << "-----------------------------------------------------" << endl;
	}
}
