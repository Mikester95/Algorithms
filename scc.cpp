#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class TarjanSCC {
    int n;
    vector<vector<int>> G, sccs;
    vector<int> wh, ind, lowlink;
    stack<int> st;
    vector<bool> inStack;
    int indCount;

    void Dfs(int x) {
        ind[x] = ++indCount;
        lowlink[x] = ind[x];
        st.push(x);
        inStack[x] = true;

        for (auto y : G[x]) {
            if (!ind[y] || inStack[y]) {
                if (!ind[y])
                    Dfs(y);
                lowlink[x] = min(lowlink[x], lowlink[y]);
            }
        }

        if (lowlink[x] == ind[x]) {
            vector<int> scc;
            int y;
            do {
                y = st.top();
                st.pop();
                inStack[y] = false;
                wh[y] = sccs.size();
                scc.push_back(y);
            } while(y != x);
            sccs.push_back(scc);
        }
    }

  public:
    TarjanSCC(const vector<vector<int>>& G) : G(G) {
        n = G.size() - 1;
        wh = ind = lowlink = vector<int>(n+1);
        inStack = vector<bool>(n+1);
        indCount = 0;

        for (int i = 1; i <= n; ++i) {
            if (!ind[i]) {
                Dfs(i);
            }
        }
    }

    vector<vector<int>> GetSCCs() {
        return sccs;
    }

    vector<int> GetWh() {
        return wh;
    }
};
