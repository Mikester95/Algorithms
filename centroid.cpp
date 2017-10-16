#include <vector>

using namespace std;

class CentroidOperations {
    int n;
    int farthest;
    vector<vector<int>> tree;
    vector<int> sub, maxSub, from, depth;
    vector<int> sol;
  public:
    void layerDfs(int x, int f) {
        from[x] = f;
        depth[x] = (f == -1 ? 0 : depth[f] + 1);

        if (depth[x] > depth[farthest]) {
            farthest = x;
        }

        for (auto y : tree[x]) {
            if (y == f || y == -1)
                continue;
            layerDfs(y, x);
        }
    }

    int getFarthest(int root) {
        if (from.size() < n + 1) {
            from.resize(n + 1);
            depth.resize(n + 1);
        }

        farthest = root;
        layerDfs(root, -1);
        return farthest;
    }

    vector<int> getDiameter(int root) {
        int start = getFarthest(root);
        int end = getFarthest(start);
        
        vector<int> diameter;
        for (int i = end; i != -1; i = from[i])
            diameter.push_back(i);

        return diameter;
    }

    vector<int> getCenters(int root) {
        auto diam = getDiameter(root);
        int half = diam.size() / 2;
        if (diam.size() & 1) {
            return {diam[half]};
        } else {
            return {diam[half-1], diam[half]};
        }
    }

    void subDfs(int x, int f) {
        sub[x] = 1;

        for (auto y : tree[x]) {
            if (y == f || y == -1)
                continue;

            subDfs(y, x);
            sub[x] += sub[y];
        }
    }

    void centroidDfs(int x, int f, int n) {
        sub[x] = 1;
        maxSub[x] = 0;

        for (auto y : tree[x]) {
            if (y == f || y == -1)
                continue;

            centroidDfs(y, x, n);
            maxSub[x] = max(maxSub[x], sub[y]);
            sub[x] += sub[y];
        }

        maxSub[x] = max(maxSub[x], n - sub[x]);

        if (sol.empty() || maxSub[sol[0]] > maxSub[x]) {
            sol = {x};
        } else if (maxSub[sol[0]] == maxSub[x]) {
            sol.push_back(x);
        }
    }

    vector<int> getCentroids(int root, int n) {
        if (sub.size() < this->n+1) {
            sub.resize(this->n+1);
            maxSub.resize(this->n+1);
        }
        sol = {};

        centroidDfs(root, 0, n);

        return sol;
    }

    void centroidDecomposition(int root, int father, int n) {
        int rememberPos = -1;
        for (int i = 0; i < tree[root].size(); ++i) {
            if (tree[root][i] == father) {
                tree[root][i] = -1;
                rememberPos = i;
            }
        }

        auto centroid = getCentroids(root, n)[0];

        // Do stuff.
        // Do stuff.

        for (auto son : tree[centroid]) {
            if (son != -1) {
                subDfs(son, centroid);
                centroidDecomposition(son, centroid, sub[son]);
            }
        }

        if (rememberPos != -1)
            tree[root][rememberPos] = father;
    }

    CentroidOperations (const vector<vector<int>>& tree) : tree(tree) {
        n = tree.size() - 1;
    }
};