#include <iostream>
#include "../tree/tree.h"
#include "../tree/tString.h"
#include "../tree/treeED.h"
#include <vector>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

unordered_map<string, int> M;

int Q = 2;

void addToMap(TreeNode *root)
{
	int length = root->postString.length();
	for (int j = 0; j <= length - int(Q); ++j) {
		string sub = root->postString.substr(j, Q);
		M[sub] += 1;
	}
}

bool PairCompare(const pair<string, int> &a, const pair<string, int> &b) {
	return M[a.first] == M[b.first]?a.first < b.first: M[a.first] < M[b.first];
}

bool ResultCompare(const pair<int, int> &a, const pair<int, int> &b) {
	return a.first == b.first ? a.second < b.second : a.first < b.first;
}

void addToList(vector<pair<string, int> > &list, TreeNode *root) {
	int length = root->postString.length();
	if (length >= int(Q)) {
		for (int j = 0; j <= length - int(Q); ++j) {
			string sub = root->postString.substr(j, Q);
			list.push_back(make_pair(sub, j));
		}
		sort(list.begin(), list.end(), PairCompare);
	}
}

void passJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
	result.clear();
	int n = f.size();
	unordered_map<string, vector<int> > L;
	for(int i = 0; i < n; ++i) {
		//get the list
		vector<pair<string, int> > list;
		addToList(list, f[i]);

		//get the prefix
		int m = list.size(), num = 0;
		vector<int> flag(max(m, 1), 0);
		flag[0] = 1;
		int k;
		for (k = 0; k < m; ++k) {
			if (num == int(threshold) + 1)
				break;
			flag[k] = 1;
			for (int j = 0; j < k; ++j)
				if (flag[j] == 1 && abs(list[k].second - list[j].second) < Q) {
					flag[k] = 0;
					break;
				}
			if (flag[k] == 1)
				++ num;
		}

		//get the candidates
		vector<int> candidates;
		unordered_map<int, bool> isDup;
		if (num < threshold + 1) {
			for (int j = 0; j <= i; ++j)
				candidates.push_back(j);
		}
		else {
			for (int j = 0; j < k; ++j)
			//if (flag[j] == 1) {
				if (L.find(list[j].first) != L.end()) {
					for (auto & l : L[list[j].first]) {
						//some pruning techniques
						//PRUNING 1
 						if (isDup.find(l) == isDup.end()) {
							candidates.push_back(l);
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
			int distance = getED(m_str1[j], m_str1[i], threshold);
			if (distance <= int(threshold)) {
				rr.id1 = i;
				rr.id2 = j;
				rr.s = distance;
				resultED.push_back(rr);
			}
		}
		*/

		//indexing all the prefix
		for (int j = 0; j < k; ++j) {
			if (L.find(list[j].first) == L.end()) {
				vector<int> temp;
				L[list[j].first] = temp;
			}
			L[list[j].first].push_back(i);
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
	int no = 0;
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

	for (int i = 1; i <= 15; ++i) {
		int edThreshold = i;
		vector<pair<int, int> > result1, result2, result;
		cout << "the threshold = " << i << endl;
		clock_t begin = clock()	;
		passJoin(f, edThreshold, result1);
		clock_t end = clock();
		cout << "the result of PassJoin = " << result1.size() << endl;
		cout << "the time of PassJoin = " << (end - begin) / CLOCKS_PER_SEC << endl;
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