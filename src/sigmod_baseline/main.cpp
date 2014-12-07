#include <iostream>
#include "../tree/treeNode.h"
#include "../tree/treeString.h"
#include "../tree/treeED.h"
#include <vector>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

unordered_map<string, int> M;

int Q = 2;
int K = 10;

void addToMap(TreeNode *root)
{
    int length = root->postOrderedString.length();
    for (int j = 0; j <= length - int(Q); ++j) {
        string sub = root->postOrderedString.substr(j, Q);
        M[sub] += 1;
    }
}

bool PairCompare(const pair<string, int> &a, const pair<string, int> &b) {
    return M[a.first] == M[b.first]?a.first < b.first: M[a.first] < M[b.first];
}

void addToList(vector<pair<string, int> > &list, TreeNode *root) {
    int length = root->postOrderedString.length();
    if (length >= int(Q)) {
        for (int j = 0; j <= length - int(Q); ++j) {
            string sub = root->postOrderedString.substr(j, Q);
            list.push_back(make_pair(sub, j));
        }
        sort(list.begin(), list.end(), PairCompare);
    }
}

void passJoin(vector<TreeNode*> &f, int threshold, vector<pair<int, int> > &result) {
    result.clear();
    int n = f.size();
    unordered_map<string, vector<int> > L;
    vector<TreeNode*> ref;
    for (int i = 0; i < K; ++i)
      ref.push_back(f[i]);
    vector<vector<int> > v(n, vector<int>(K));
    for (int i = 0; i< n; ++i)
      for (int j = 0; j < K; ++j)
        v[i][j] = treeED(f[i], ref[j], threshold + 1);
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
            for (int j = 0; j < i; ++j) {
              int lt = 0;
              for (int kk = 0; kk < K; ++kk)
                lt = max(abs(v[i][kk] - v[j][kk]), lt);
              if (lt <= threshold) 
                candidates.push_back(j);
            }
        }
        else {
            for (int j = 0; j < k; ++j)
                //if (flag[j] == 1) {
                if (L.find(list[j].first) != L.end()) {
                    for (auto & l : L[list[j].first]) {
                        //some pruning techniques
                        //PRUNING 1
                        if (isDup.find(l) == isDup.end()) {
                            int lt = 0;
                            for (int kk = 0; kk < K; ++kk)
                              lt = max(abs(v[i][kk] - v[l][kk]), lt);
                            if (lt <= threshold)
                              candidates.push_back(l);
                            isDup[l] = true;
                        }
                        //PRUNING 2

                    }
                }
            //}
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
    TreeNode *tree = readFile(argv[1]);
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
    
    for (int i = 1; i <= 15; ++i) {
        int edThreshold = i;
        vector<pair<int, int> > result1, result2, result;
        auto t1 = chrono::system_clock::now();
        passJoin(f, edThreshold, result1);
        auto t2 = chrono::system_clock::now();
        cout << "the result of PassJoin = " << result1.size() << endl;
        cout << "the time of PassJoin = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
        t1 = chrono::system_clock::now();
        for (auto & j : result1)
            if (getED(f[j.first]->postOrderedString, f[j.second]->postOrderedString, edThreshold) <= edThreshold)
                result2.push_back(make_pair(j.first, j.second));
        t2 = chrono::system_clock::now();
        cout << "the result of String ED = " << result2.size() << endl;
        cout << "the time of String ED = " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << endl;
        t1 = chrono::system_clock::now();
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
