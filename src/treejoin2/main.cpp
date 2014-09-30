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

void addToMap(TreeNode *root) {
	FM[root->eulerString.back().second] += 1;
	for (auto & i : root->children)
		addToMap(i);
}

void addToList(vector<pair<TreeNode*, int> > &list, TreeNode *root) {
	list.push_back(make_pair(root, FM[root->eulerString.back().second]));
	for (auto & i : root->children)
		addToList(list, i);
}

bool FrequencyCompare(const pair<TreeNode*, int> &a, const pair<TreeNode*, int> &b) {
	return a.second == b.second ? (a.first)->eulerString.back().first.length() > (b.first)->eulerString.back().first.length() : a.second < b.second;
}

void TreeJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = f.size();
	unordered_map<unsigned int, vector<int> > L;
	for (int i = 0; i < n; ++i) {
		//get the list
		vector<pair<TreeNode*, int> > list;
		addToList(list, f[i]);
		sort(list.begin(), list.end(), FrequencyCompare);

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
			for (int j = 0; j < i; ++j)
				if (abs(int(f[j]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
					candidates.push_back(j);
				}
		}
		else {
			for (int j = 0; j < k; ++j)
				//if (flag[j] == 1) {
					if (L.find((list[j].first)->eulerString.back().second) != L.end()) {
						for (auto & l : L[(list[j].first)->eulerString.back().second]) {
	 						if (isDup.find(l) == isDup.end()) {
	 							if (abs(int(f[l]->postOrderedString.length()) - int(f[i]->postOrderedString.length())) < threshold) {
									candidates.push_back(l);
								}
								isDup[l] = true;
							}
						}
					}
				//}
		}
		for (auto & j : candidates) {
			result.push_back(make_pair(i, j));
		}

		//indexing all the prefix
		for (int j = 0; j < k; ++j) {
			if (L.find((list[j].first)->eulerString.back().second) == L.end()) {
				vector<int> temp;
				L[(list[j].first)->eulerString.back().second] = temp;
			}
			L[(list[j].first)->eulerString.back().second].push_back(i);
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

	for (int i = 5; i <= 5; ++i) {
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
