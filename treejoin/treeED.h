#ifndef TREEED_H
#define TREEED_H

#include <cstring>
#include <map>

#define LIMIT 20

string getPostorderedString(TreeNode *root) {
	string ret = "";
	for (auto & i : root->child)
		ret.append(getPostorderedString(i) + "$");
	ret.append(root->label);
	return ret;
}

int generatePostorderedString(TreeNode *root, const char *filename) {
	cout << "start generating postordered string" << endl;
	ofstream fout(filename);
	int count = 0;
	for (auto & i : root->child) {
		string temp = getPostorderedString(i);
		if (temp != "" && temp[0] != '$') {
			fout << temp << endl;
			++count;
		}
		tString t(temp);
		i->postString = t;
	}
	fout.close();
	cout << "generating finished" << endl;
	return count;
}

int costFunc(const unsigned int &a, const unsigned int &b) {
	if (a == b)
		return 0;
	return 1;	
}

int dfs(TreeNode *f1, TreeNode *f2, int **ans, int **ted, int sum1, int sum2, int threshold);

int TED(TreeNode *f1, TreeNode *f2, int **ted, int threshold) {
	int n = f1->sum, m = f2->sum;
	if (abs(n - m) > threshold)
		return LIMIT + 1;
	if (ted[f1->rank][f2->rank] != -1) {
		return ted[f1->rank][f2->rank];
	}
	int **ans = new int*[n];
	for (int i = 0; i < n; ++i) {
		ans[i] = new int[m];
		memset(ans[i], -1, sizeof(int) * m);
	}
	dfs(f1, f2, ans, ted, 0, 0, threshold);
	int ret = ans[0][0];
	for (int i = 0; i < n; ++i)
		delete ans[i];
	delete [] ans;
	ted[f1->rank][f2->rank] = ret;
	return ret;
}

int treeED(TreeNode *f1, TreeNode *f2, int threshold) {
	int n = f1->sum, m = f2->sum;
	if (abs(n - m) > threshold)
		return LIMIT + 1;
	int **ans = new int*[n];
	for (int i = 0; i < n; ++i) {
		ans[i] = new int[m];
		memset(ans[i], -1, sizeof(int) * m);
	}
	int **ted = new int*[n];
	for (int i = 0; i < n; ++i) {
		ted[i] = new int[m];
		memset(ted[i], -1, sizeof(int) * m);
	}
	dfs(f1, f2, ans, ted, 0, 0, threshold);
	int ret = ans[0][0];
	for (int i = 0; i < n; ++i)
		delete ans[i];
	delete [] ans;
	delete [] ted;
	return ret;
}

int dfs(TreeNode *f1, TreeNode *f2, int **ans, int **ted, int sum1, int sum2, int threshold) {
	if (f1 == NULL && f2 == NULL)
		return 0;
	if (ans[sum1][sum2] != -1)
		return ans[sum1][sum2];
	if (threshold < 0)
		return LIMIT + 1;
	if (f1->getSize() == 0 || f2->getSize() == 0)
		return max(f1->getSize(), f2->getSize());
	if (abs(f1->sum - sum1 - f2->sum + sum2) > threshold)
		return LIMIT + 1;
	int ret = LIMIT + 1;
	//cout << "0" << endl;
	if (f1->getSize() > 0) {
		int temp = f1->getSize();
		TreeNode *v = f1->deleteRightmostChild();
		ret = min(ret, dfs(f1, f2, ans, ted, sum1 + 1, sum2, threshold - costFunc(v->llabel, 0)) + costFunc(v->llabel, 0));
		while (f1->getSize() > temp - 1)
			f1->deleteRightmostTree();
		f1->insertChild(v);
	}
	//cout << "1" << endl;
	if (f2->getSize() > 0) {
		int temp = f2->getSize();
		TreeNode *w = f2->deleteRightmostChild();
		ret = min(ret, dfs(f1, f2, ans, ted, sum1, sum2 + 1, threshold- costFunc(0, w->llabel)) + costFunc(0, w->llabel));
		while (f2->getSize() > temp - 1)
			f2->deleteRightmostTree();
		f2->insertChild(w);
	}
	//cout << "2" << endl;
	if (f1->getSize() > 0 && f2->getSize() > 0) {
		TreeNode *v = f1->deleteRightmostTree();
		TreeNode *w = f2->deleteRightmostTree();
		int distance = TED(v, w, ted, threshold);
		ret = min(ret, distance + dfs(f1, f2, ans, ted, sum1 + v->sum, sum2 + w->sum, threshold - distance - costFunc(v->llabel, w->llabel)) + costFunc(v->llabel, w->llabel));
		f1->insertChild(v);
		f2->insertChild(w);
	}
	ans[sum1][sum2] = ret;
	return ret;
}

void calcSum(TreeNode *t) {
	map<int, int> m;
	for (auto & i : t->child) {
		m[i->sum] += 1;
	}
	for (auto & j : m) {
		cout << j.first << endl;
	}
	for (auto & j : m) {
		cout << j.second << endl;
	}
}

int getED(tString &a, tString &b, int threshold)
{
	double dis = 0;
	int len_a = a.length(), len_b = b.length();
	vector<int> d0(len_b + 1, 0);
	vector<int> d1(len_b + 1, 0);
	dis = threshold + 1;
	if (abs(len_a - len_b) > threshold)
		return dis;
	for(int i = 0; i <= len_a; ++ i)
    {
       	int l = max(0, i - (int)threshold), r = min(len_b, i + (int)threshold);
        int minDis = threshold + 1;
        for(int j = l; j <= r; ++ j)
        {
        	if(i == 0)
                d1[j] = j;
            else if(j == 0)
               	d1[j] = i;
        	else 
        	{
        		if(a.getElement(i - 1) == b.getElement(j - 1))
                    d1[j] = d0[j - 1];
                else
                   	d1[j] = d0[j - 1] + 1;
        		if(j > l) d1[j] = min(d1[j], d1[j - 1] + 1);
            	if(j < i + threshold) d1[j] = min(d1[j], d0[j] + 1);
            }
            minDis = min(minDis, d1[j]);	
        }
        if(minDis > threshold)
        	return dis;
        swap(d0, d1);
   	}
    dis = d0[len_b];
    return dis;
}


#endif