#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class Tarjan {
    int n;
    vector<vector<int>> G, ccs;
    vector<int> wh, ind, lowlink;
    stack<int> st;
    vector<bool> inStack;
    int indCount;

    void dfs(int x) {
        ind[x] = ++indCount;
        lowlink[x] = ind[x];
        st.push(x);
        inStack[x] = true;

        for (auto y : G[x]) {
            if (!ind[y] || inStack[y]) {
                if (!ind[y])
                    dfs(y);
                lowlink[x] = min(lowlink[x], lowlink[y]);
            }
        }

        if (lowlink[x] == ind[x]) {
            vector<int> cc;
            int y;
            do {
                y = st.top();
                st.pop();
                inStack[y] = false;
                wh[y] = ccs.size();
                cc.push_back(y);
            } while(y != x);
            ccs.push_back(cc);
        }
    }

  public:
    Tarjan(const vector<vector<int>>& G) : G(G) {
        n = G.size() - 1;
        wh = ind = lowlink = vector<int>(n+1);
        inStack = vector<bool>(n+1);
        indCount = 0;

        for (int i = 1; i <= n; ++i) {
            if (!ind[i])
                dfs(i);
        }
    }

    vector<vector<int>> getCcs() {
        return ccs;
    }

    vector<int> getWh() {
        return wh;
    }
};
