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
vector<vector<unsigned int> > GM;
vector<vector<unsigned int> > BGM;
vector<vector<unsigned int> > TM;
vector<vector<unsigned int> > NM;
vector<unsigned int> V;
vector<unsigned int> BV;

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
	return ma == mb ? ((a.first)->eulerString[a.second]).second < ((b.first)->eulerString[b.second]).second : ma < mb;
}

bool NodeCompare(const unsigned int a, const unsigned int b) {
	return BV[a] < BV[b];
}

inline void calcRank(TreeNode *root, int step, int index) {
	if (step < 0)
		return;
	min_rank = min(root->rank, min_rank);
	max_rank = max(root->rank, max_rank);
	for (auto & j : root->children)
		calcRank(j, step - 1, index);
}

inline void pushToMap(TreeNode *root, int step, int index) {
	if (step < 0)
		return;
	if (GM[index][root->rank - min_rank] == 0) {
		GM[index][root->rank - min_rank] = 1;
		V[root->rank - min_rank] += 1;
	}
	for (auto & j : root->children)
		pushToMap(j, step - 1, index);
}

inline int findOverlapNodes(vector<pair<TreeNode*, int> > &list, int n, int threshold) {
	GM[n - 1].resize(max_rank - min_rank + 1);
	pushToMap(list[n - 1].first, list[n - 1].second, n - 1);
	for (int j = 0; j <= int(max_rank - min_rank); ++j) {
		if (GM[n - 1][j] == 1) {
			TM[n - 1].push_back(j);
			NM[j].push_back(n - 1);
		}
	}

	/*

	BGM = GM;
	BV = V;

	vector<unsigned int> v;
	v.reserve(max_rank - min_rank + 1);
	for (int i = 0; i <= int(max_rank - min_rank); ++i)
		if (BV[i] > 0)
			v.push_back(i);
	int ret = 0, counter = 0;
	while (!v.empty() && ret < threshold + 1 && counter < n) {
		sort(v.begin(), v.end(), NodeCompare);
		for (int i = 0; i < n; ++i) {
			if (BGM[i][v.back()] > 0) {
				BGM[i][v.back()] = 0;
				for (int j = 0; j <= int(max_rank - min_rank); ++j) {
					if (BGM[i][j] == 1) {
						BGM[i][j] = 0;
						BV[j] -= 1;
					}
				}
				++counter;
			}
		}
		v.pop_back();
		++ret;
	}
	*/

	//{
		BGM = GM;
		BV = V;
		vector<unsigned int> v;
		v.reserve(max_rank - min_rank + 1);
		for (int i = 0; i <= int(max_rank - min_rank); ++i)
			if (BV[i] > 0)
				v.push_back(i);

		int ret = 0, counter = 0;
		while (!v.empty() && ret < threshold + 1 && counter < n) {
			sort(v.begin(), v.end(), NodeCompare);
			for (auto i : NM[v.back()]) {
				if (BGM[i][v.back()] > 0) {
					BGM[i][v.back()] = 0;
					for (auto j : TM[i]) {
						if (BGM[i][j] == 1) {
							BGM[i][j] = 0;
							BV[j] -= 1;
						}
					}
					++counter;
				}
			}
			v.pop_back();
			++ret;
		}
	//}

	return ret;
}

inline void TreeJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = f.size();
	unordered_map<unsigned int, vector<int> > L;
	long long sum_k = 0;
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
		vector<pair<TreeNode*, int> > &list = PL[i];

		//get the prefix
		// linear method
		min_rank = 1000000000;
		max_rank = 0;
		for (int j = 0; j < int(list.size()); ++j) {
			calcRank(list[j].first, list[j].second, j);
		}
		GM.clear();
		GM.resize(list.size());
		TM.clear();
		TM.resize(list.size());
		NM.clear();
		NM.resize(max_rank - min_rank + 1);
		V.clear();
		V.resize(max_rank - min_rank + 1);
		//cout << max_rank - min_rank + 1 << endl;

		/*
		int l = 1;
		for (; l <= int(list.size()); ++l)
			if (findOverlapNodes(list, l, threshold) >= threshold + 1) {
				break;
			}
		*/
		int l = min(25, int(list.size()));

		//get the candidates
		vector<int> candidates;
		unordered_set<int> isDup;
		sum_k += l;
		int k = l;
		cout << i << "  " << l << endl;

		for (int j = 0; j < k; ++j) {
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
		for (auto & j : candidates) {
			result.push_back(make_pair(i, j));
		}

		//indexing all the prefix
		for (int j = 0; j < k; ++j) {
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

	for (int i = 10; i <= 10; ++i) {
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
		t2 = chrono::system_clock::now();
		cout << "the result of String ED = " << result2.size() << endl;
		cout << "the time of String ED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		t1 = chrono::system_clock::now();
		for (auto & j : result2)
			if (j.first == j.second || treeED(f[j.first], f[j.second], edThreshold) <= edThreshold)
				result.push_back(make_pair(j.first, j.second));
		t2 = chrono::system_clock::now();
		cout << "the number of the answers = " << result.size() << endl;
		cout << "the time of TreeED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		cout << "-----------------------------------------------------" << endl;
	}
}
