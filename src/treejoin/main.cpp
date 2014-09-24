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

unordered_map<int, int> M;

void addToMap(TreeNode *root) {
	for (auto & i : root->eulerString)
		M[i.second] += 1;
	for (auto & i : root->children)
		addToMap(i);
}

void addToList(vector<pair<TreeNode*, int> > &list, TreeNode *root) {
	for (int i = 0; i < int(root->eulerString.size()); ++i)
		list.push_back(make_pair(root, i));
	for (auto & i : root->children)
		addToList(list, i);
}

bool PairCompare(const pair<TreeNode*, int> &a, const pair<TreeNode*, int> &b) {
	int ma = M[((a.first)->eulerString[a.second]).second], mb = M[((b.first)->eulerString[b.second]).second];
	return ma == mb ? ((a.first)->eulerString[a.second]).second < ((b.first)->eulerString[b.second]).second : ma < mb;
}

void addToMap2(unordered_map<int, int> &m, TreeNode *root, int step) {
	if (step < 0)
		return;
	mm[root->rank] += 1;
	for (int i = 0; i <= step; ++i) {
		for (auto & j : root->children)
			addToMap2(m, j, step - 1);
	}
}

int findOverlapNodes(vector<pair<TreeNode*, int> > &list, int n) {
	unordered_map<int, int> m;
	for (int i = 0; i < n; ++i) {
		addToMap2(mm, list[i].first, list[i].second);
	}
	
	return 1;
}

void TreeJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = f.size();
	unordered_map<int, vector<int> > L;
	for (int i = 0; i < n; ++i) {
		// nodes are less than threshold + 1
		if (f[i]->sum < threshold + 1) {
			for (int j = 0; j < i; ++j)
				if (abs(int(f[j]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
					result.push_back(make_pair(i, j));
				}
			continue;
		}

		// get the list
		vector<pair<TreeNode*, int> > list;
		addToList(list, f[i]);
		sort(list.begin(), list.end(), PairCompare);

		//get the prefix
		int l = 1, r = int(list.size()), m = 0;
		while (l < r) {
			m = (l + r) >> 1;
			if (findOverlapNodes(list, m) >= threshold + 1) {
				r = m - 1;
			} else {
				l = m + 1;
			}
		}

		//get the candidates
		vector<int> candidates;
		unordered_map<int, bool> isDup;
		int k = l;
		for (int j = 0; j < k; ++j) {
			if (L.find(((list[j].first)->eulerString[list[j].second]).second) != L.end()) {
				for (auto & l : L[((list[j].first)->eulerString[list[j].second]).second]) {
	 				if (isDup.find(l) == isDup.end()) {
	 					if (abs(int(f[l]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
							candidates.push_back(l);
						}
						isDup[l] = true;
					}
				}
			}
		}
		for (auto & j : candidates) {
			result.push_back(make_pair(i, j));
		}

		//indexing all the prefix
		for (int j = 0; j < k; ++j) {
			if (L.find(((list[j].first)->eulerString[list[j].second]).second) == L.end()) {
				vector<int> temp;
				L[((list[j].first)->eulerString[list[j].second]).second] = temp;
			}
			L[((list[j].first)->eulerString[list[j].second]).second].push_back(i);
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Usage : " << endl;
		cout << "main filename" << endl;
		return 0;
	}
	cout << "reading..." << endl;
	TreeNode *root = readFile(argv[1]);
	cout << "reading finished" << endl;

	int totalNum = generatePostOrderedString(root, "strings.txt");
	cout << "totalNum = " << totalNum << endl;

	vector<TreeNode*> f;
	int totalSum = 0;
	for (int i = 0; i < totalNum; ++i) {
		f.push_back(root->children[i]);
		root->children[i]->calc();
		totalSum += root->children[i]->sum;
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
