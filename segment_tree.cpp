#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

typedef long long int64;

class SegTree {
    int n, l, r;
    int64 upd, res;
    vector<int64> tree, lazy;
    vector<int64> source;

    inline void recon(int node) {
        tree[node] = max(tree[node<<1], tree[node<<1^1]);
    }

    inline void applyUpd (const int64& upd, int node, int left, int right) {
        tree[node] = upd;
        lazy[node] = upd;
    }

    inline void unloadLazy(int node, int left, int right) {
        if (lazy[node] == -1)
            return;
        int mid = (left + right) >> 1;
        if (left != right) {
            applyUpd(lazy[node], node<<1, left, mid);
            applyUpd(lazy[node], node<<1^1, mid+1, right);
        }
        lazy[node] = -1;
    }

    void createTree(int node, int left, int right) {
        lazy[node] = -1;
        if (left == right) {
            tree[node] = source[left - 1];
            return;
        }

        int mid = (left + right) >> 1;
        createTree(node<<1, left, mid);
        createTree(node<<1^1, mid+1, right);
        recon(node);
    }

public:
    SegTree(const vector<int64>& source, bool hasLazy=false) 
    : source(source), hasLazy(hasLazy) {
        n = source.size();
        tree.resize(4*n + 1);
        lazy.resize(4*n + 1);
        createTree(1, 1, n);
    }

private:
    void queryRec(int node, int left, int right) {
        if (l <= left && right <= r) {
            res = max(res, tree[node]);
            return;
        }
        unloadLazy(node, left, right);

        int mid = (left + right) >> 1;
        if (l <= mid)
            queryRec(node<<1, left, mid);
        if (r > mid)
            queryRec(node<<1^1, mid+1, right);
    }

    void updateRec(int node, int left, int right) {
        if (l <= left && right <= r) {
            applyUpd(upd, node, left, right);
            return;
        }
        unloadLazy(node, left, right);

        int mid = (left + right) >> 1;
        if (l <= mid)
            updateRec(node<<1, left, mid);
        if (r > mid)
            updateRec(node<<1^1, mid+1, right);
        recon(node);
    }

public:
    int64 query(int l, int r) {
        this->l = l;
        this->r = r;
        res = -1;
        queryRec(1, 1, n);
        return res;
    }

    void update(int l, int r, const int64& upd) {
        this->l = l;
        this->r = r;
        this->upd = upd;
        updateRec(1, 1, n);
    }
};

int main() {
    int n, m;
    ifstream fin("arbint.in");
    ofstream fout("arbint.out");

    fin >> n >> m;

    vector<int64> a;
    a.reserve(n);

    for (int i = 1; i <= n; ++i) {
        int64 x;
        fin >> x;
        a.push_back(x);
    }

    SegTree segTree(a);

    for (int i = 1; i <= m; ++i) {
        int t, a, b;
        fin >> t >> a >> b;
        if (t == 0) {
            fout << segTree.query(a, b) << "\n";
        } else {
            segTree.update(a, a, b);
        }
    }
}