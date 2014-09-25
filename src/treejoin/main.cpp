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


using namespace std;

unordered_map<unsigned int, int> FM;
unordered_map<unsigned int, unordered_set<unsigned int> > NM;
unordered_map<unsigned int, unordered_set<unsigned int> > LM;

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

bool FrequencyCompare(const pair<TreeNode*, int> &a, const pair<TreeNode*, int> &b) {
	int ma = FM[((a.first)->eulerString[a.second]).second], mb = FM[((b.first)->eulerString[b.second]).second];
	return ma == mb ? ((a.first)->eulerString[a.second]).second < ((b.first)->eulerString[b.second]).second : ma < mb;
}

bool NodeCompare(const unsigned int a, const unsigned int b) {
	return NM[a].size() < NM[b].size();
}

void pushToMap(TreeNode *root, int step, int index) {
	if (step < 0)
		return;
	NM[root->rank].insert(index);
	LM[index].insert(root->rank);
	for (auto & j : root->children)
		pushToMap(j, step - 1, index);
}

int findOverlapNodes(vector<pair<TreeNode*, int> > &list, int n, int threshold) {
	NM.clear();
	LM.clear();
	for (int i = 0; i < n; ++i) {
		pushToMap(list[i].first, list[i].second, i);
	}

	vector<unsigned int> v; 
	for (auto & i : NM)
		v.push_back(i.first);

	int ret = 0, counter = 0;
	while (!v.empty() && ret < threshold + 1 && counter < n) {
		sort(v.begin(), v.end(), NodeCompare);
		for (auto & i : NM[v.back()]) {
			for (auto & j : LM[i]) {
				if (j != v[v.size() - 1])
					NM[j].erase(i);
			}
			++counter;
		}
		v.pop_back();
		++ret;
	}
	return ret;
}

void TreeJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	FM.clear();
	result.clear();
	int n = f.size();
	unordered_map<unsigned int, vector<int> > L;
	for (int i = 0; i < n; ++i) {
		cout << i << endl;
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
		sort(list.begin(), list.end(), FrequencyCompare);

		//get the prefix
		int l = 1, r = int(list.size()), m = 0;
		while (l <= r) {
			m = (l + r) >> 1;
			if (findOverlapNodes(list, m, threshold) >= threshold + 1) {
				r = m - 1;
			} else {
				l = m + 1;
			}
			//cout << l << "  " << m << "  " << r << endl;
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

		for (int j = 0; j < i; ++j) {
			if (isDup.find(j) == isDup.end()) {
				if (treeED(f[i], f[j], threshold) <= threshold) {
					cout << i << " " << j << endl;
					cout << f[i]->eulerString.back().first << endl;
					cout << f[j]->eulerString.back().first << endl;
					assert(1 == 0);
				}
			}
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

	for (int i = 1; i <= 1; ++i) {
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
