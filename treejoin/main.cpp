#include <iostream>
#include "tree.h"
#include "tString.h"
#include "treeED.h"
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cassert>

using namespace std;

unordered_map<string, int> M;

void addToMap(TreeNode *root) {
	M[root->eulerString] += 1;
	for (auto & i : root->child)
		addToMap(i);
}

void addToList(vector<pair<TreeNode*, int> > &list, TreeNode *root) {
	list.push_back(make_pair(root, M[root->eulerString]));
	for (auto & i : root->child)
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
	 							if (abs(int(f[l]->postString.length()) - int(f[i]->postString.length())) < threshold) {
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

	int totalNum = generatePostorderedString(tree, "strings.txt");
	cout << "totalNum = " << totalNum << endl;

	vector<TreeNode*> f;
	int totalSum = 0;
	for (int i = 0; i < totalNum; ++i) {
		f.push_back(tree->child[i]);
		tree->child[i]->calc();
		totalSum += tree->child[i]->sum;
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
		int counter = 0;
		for (auto & j : result1)
			if (getED(f[j.first]->postString, f[j.second]->postString, edThreshold) <= edThreshold)
				result2.push_back(make_pair(j.first, j.second));
			/*
			else if (counter < 10000) {
				counter ++;
				cout << getED(f[j.first]->postString, f[j.second]->postString, edThreshold) + costFunc(f[j.first]->llabel, f[j.second]->llabel);
				cout << ' ' << f[j.first]->postString.length() << ' ' << f[j.second]->postString.length() << endl;
				cout << f[j.first]->sum << ' ' << f[j.second]->sum << endl;
				cout << f[j.first]->label << endl;
				cout << f[j.first]->postString.ssubstr(0, f[j.first]->postString.length()) << endl;
				cout << f[j.second]->postString.ssubstr(0, f[j.second]->postString.length()) << endl;
				cout << f[j.first]->eulerString << endl;
				cout << f[j.second]->eulerString << endl;
			}
			*/
		end = clock();
		cout << "the result of String ED = " << result2.size() << endl;
		cout << "the time of String ED = " << (end - begin) / CLOCKS_PER_SEC << endl;
		begin = clock();
		for (auto & j : result2)
			if (j.first == j.second || treeED(f[j.first], f[j.second], edThreshold) + costFunc(f[j.first]->llabel, f[j.second]->llabel) <= edThreshold)
				result.push_back(make_pair(j.first, j.second));
		end = clock();
		cout << "the number of the answers = " << result.size() << endl;
		cout << "the time of TreeED = " << (end - begin) / CLOCKS_PER_SEC << endl;
		cout << "-----------------------------------------------------" << endl;
	}
}
