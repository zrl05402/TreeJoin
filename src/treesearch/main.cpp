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

const int MAX_THRESHOLD = 10;

unordered_map<unsigned int, int> FM;
unordered_map<unsigned int, unordered_set<unsigned int> > NM;
unordered_map<unsigned int, unordered_set<unsigned int> > LM;
unordered_map<unsigned int, vector<vector<int> > > GL;

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

void preprocess(vector<TreeNode*> &data, vector<vector<pair<TreeNode*, int> > > all) {
	int n = data.size();
	for (int i = 0; i < n; ++i) {
		// get the list
		vector<pair<TreeNode*, int> > &list = all[i];

		// get the prefix
		int l = 1, t = 0;
		vector<int> prefix(MAX_THRESHOLD + 1, 0);
		while (l <= int(list.size()) && t <= MAX_THRESHOLD) {
			if (findOverlapNodes(list, l, t) >= t + 1) {
				prefix[t] = l - 1;
				++ l;
				++ t;
			} else {
				++ l;
			}
		}

		//indexing all the prefix
		for (int t = 0; t <= MAX_THRESHOLD; ++t) {
			int prev = t > 0 ? prefix[t - 1] : 0;
			for (int j = prev; j < prefix[t]; ++j) {
				if (GL.find(((list[j].first)->eulerString[list[j].second]).second) == GL.end()) {
					GL[((list[j].first)->eulerString[list[j].second]).second].resize(MAX_THRESHOLD + 1);
				}
				GL[((list[j].first)->eulerString[list[j].second]).second][t].push_back(i);
			}
		}
	}
}

void TreeSearch(vector<TreeNode*> &data, vector<TreeNode*> &query, vector<vector<pair<TreeNode*, int> > > &all, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = query.size();
	for (int i = 0; i < n; ++i) {
		// nodes are less than threshold + 1
		if (query[i]->sum < threshold + 1) {
			for (int j = 0; j < int(data.size()); ++j)
				if (abs(int(data[j]->postOrderedString.length()) - int(query[i]->postOrderedString.length())) < threshold) {
					result.push_back(make_pair(i, j));
				}
			continue;
		}

		// get the list
		vector<pair<TreeNode*, int> > &list = all[i];

		int l = 1;
		for (; l <= int(list.size()); ++l)
			if (findOverlapNodes(list, l, threshold) >= threshold + 1) {
				break;
			}

		//get the candidates
		vector<int> candidates;
		unordered_set<int> isDup;

		for (int j = 0; j < l; ++j) {
			if (GL.find(((list[j].first)->eulerString[list[j].second]).second) != GL.end()) {
				for (int k = 0; k <= threshold; ++k) {
					for (auto & l : GL[((list[j].first)->eulerString[list[j].second]).second][k]) {
		 					if (isDup.find(l) == isDup.end()) {
		 						if (abs(int(data[l]->postOrderedString.length()) - int(query[i]->postOrderedString.length())) < threshold) {
									candidates.push_back(l);
								}
								isDup.insert(l);
							}
					}
				}
			}
		}	
		for (auto & j : candidates) {
			result.push_back(make_pair(i, j));
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

	vector<TreeNode*> data;
	int totalSum = 0;
	vector<vector<pair<TreeNode*, int> > > all(totalNum);
	for (int i = 0; i < totalNum; ++i) {
		data.push_back(root->children[i]);
		root->children[i]->calc();
		totalSum += root->children[i]->sum;
		addToMap(data[i]);
		addToList(all[i], data[i]);
		sort(all[i].begin(), all[i].end(), FrequencyCompare);
	}
	cout << "totalSum = " << totalSum << endl;

	//calcSum(tree);

	// preprocess
	cout << "preprocess..." << endl;
	auto t1 = chrono::system_clock::now();
	preprocess(data, all);
	auto t2 = chrono::system_clock::now();
	cout << "the time of preprocess = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;

	for (int i = 10; i <= 10; ++i) {
		vector<TreeNode*> query;
		for (int j = 0; j < 1000; ++j)
			query.push_back(data[j]);
		vector<pair<int, int> > result1, result2, result;
		auto t1 = chrono::system_clock::now();
		TreeSearch(data, query, all, i, result1);
		auto t2 = chrono::system_clock::now();
		cout << "the result of prefix filter = " << result1.size() << endl;
		cout << "the time of prefix filter = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		t1 = chrono::system_clock::now();
		for (auto & j : result1)
			if (getED(query[j.first]->postOrderedString, data[j.second]->postOrderedString, i) <= i)
				result2.push_back(make_pair(j.first, j.second));
		t2 = chrono::system_clock::now();
		cout << "the result of String ED = " << result2.size() << endl;
		cout << "the time of String ED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		t1 = chrono::system_clock::now();
		for (auto & j : result2)
			if (j.first == j.second || treeED(query[j.first], data[j.second], i) <= i)
				result.push_back(make_pair(j.first, j.second));
		t2 = chrono::system_clock::now();
		cout << "the number of the answers = " << result.size() << endl;
		cout << "the time of TreeED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
		cout << "-----------------------------------------------------" << endl;
	}
}
