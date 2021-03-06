#include <iostream>
#include "../tree/treeNode.h"
#include "../tree/treeString.h"
#include "../tree/treeED.h"
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <cassert>
#include <chrono>
#include <limits.h>

using namespace std;

unordered_map<unsigned int, int> FM;
vector<vector<pair<TreeNode*, int> > > PL;
unordered_map<string, bool> NS;

unsigned int min_rank, max_rank;

void addToMap(TreeNode *root) {
	for (auto & i : root->eulerString)
		FM[i.second] += 1;
	for (auto & i : root->children)
		addToMap(i);
}

void addToList(vector<pair<TreeNode*, int> > &list, TreeNode *root) {
	for (int i = 0; i < int(root->eulerString.size()); ++i)
		list.push_back(make_pair(root, i));
	for (auto & i : root->children)
		addToList(list, i);
}

inline bool FrequencyCompare(const pair<TreeNode*, int> &a, const pair<TreeNode*, int> &b) {
	int ma = FM[((a.first)->eulerString[a.second]).second], mb = FM[((b.first)->eulerString[b.second]).second];
	return ma == mb ? ((a.first)->eulerString[a.second]).first < ((b.first)->eulerString[b.second]).first : ma < mb;
}

void calcRank(TreeNode *root, int step, int index) {
	if (step < 0)
		return;
	min_rank = min(root->rank, min_rank);
	max_rank = max(root->rank, max_rank);
	for (auto & j : root->children)
		calcRank(j, step - 1, index);
}

bool pushToMap(TreeNode *root, string &node, int step) {
	if (step < 0)
		return true;
	if (NS[root->label]) {
		return false;
	} else {
		node = root->label;
	}
	for (auto & j : root->children) {
		if (!pushToMap(j, node, step - 1)) {
			return false;
		}
	}
	return true;
}

int findOverlapNodes(vector<pair<TreeNode*, int> > &list, int &counter, int n, int threshold) {
	string node = "";
	if (!pushToMap(list[n - 1].first, node, list[n - 1].second))
		return counter;
	NS[node] = true;
	++counter;
	return counter;
}

void TreeJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = f.size();
	unordered_map<unsigned int, vector<int> > L;
	long long sum_k = 0;
	vector<int> less;
	for (int i = 0; i < n; ++i) {
		// nodes are less than threshold + 1
		if (f[i]->sum < threshold + 1) {
			less.push_back(i);
			for (int j = 0; j < i; ++j)
				if (abs(int(f[j]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
					result.push_back(make_pair(i, j));
				}
			continue;
		}

		// get the list
		vector<pair<TreeNode*, int> > &list = PL[i];

		//get the prefix
		// linear method
		NS.clear();
		int l = 1, counter = 0;
		for (; l <= int(list.size()); ++l)
			if (findOverlapNodes(list, counter, l, threshold) >= threshold + 1) {
				break;
			}
		if (l > int(list.size())) {
			less.push_back(i);
			continue;
		}

		//get the candidates
		vector<int> candidates;
		unordered_set<int> isDup;
		sum_k += l;

		for (int j = 0; j < l; ++j) {
			if (L.find(((list[j].first)->eulerString[list[j].second]).second) != L.end()) {
				for (auto & l : L[((list[j].first)->eulerString[list[j].second]).second]) {
	 				if (isDup.find(l) == isDup.end()) {
	 					if (abs(int(f[l]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
							candidates.push_back(l);
						}
						isDup.insert(l);
					}
				}
			}
		}
		for (auto j : candidates) {
			result.push_back(make_pair(i, j));
		}
		for (auto j : less) {
			if (abs(int(f[j]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
				result.push_back(make_pair(i, j));
			}
		}

		//indexing all the prefix
		for (int j = 0; j < l; ++j) {
			L[((list[j].first)->eulerString[list[j].second]).second].push_back(i);
		}
	}
	cout << (double)sum_k / n << endl;
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
	PL.resize(totalNum);
	for (int i = 0; i < totalNum; ++i) {
		f.push_back(root->children[i]);
		root->children[i]->calc();
		totalSum += root->children[i]->sum;
		addToMap(f[i]);
		addToList(PL[i], f[i]);
		sort(PL[i].begin(), PL[i].end(), FrequencyCompare);
	}
	cout << "totalSum = " << totalSum << endl;

	//calcSum(tree);

	int thres = atoi(argv[2]);

	for (int i = thres; i <= thres; ++i) {
		int edThreshold = i;
		vector<pair<int, int> > result1, result2, result;
		auto t1 = chrono::system_clock::now();
		TreeJoin(f, i, result1);
		auto t2 = chrono::system_clock::now();
		cout << "the result of prefix filter = " << result1.size() << endl;
		cout << "the time of prefix filter = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		t1 = chrono::system_clock::now();
		for (auto & j : result1)
			if (getED(f[j.first]->postOrderedString, f[j.second]->postOrderedString, edThreshold) <= edThreshold)
				result2.push_back(make_pair(j.first, j.second));
		//t2 = chrono::system_clock::now();
		//cout << "the result of String ED = " << result2.size() << endl;
		//cout << "the time of String ED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		//t1 = chrono::system_clock::now();
		for (auto & j : result2)
			if (treeED(f[j.first], f[j.second], edThreshold) <= edThreshold) {
				result.push_back(make_pair(j.first, j.second));
				//cout << j.first << " " << j.second << endl;
			}
		t2 = chrono::system_clock::now();
		cout << "the number of the answers = " << result.size() << endl;
		cout << "the time of TreeED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		cout << "-----------------------------------------------------" << endl;
	}
}
