#include <vector>
#include <stack>
#include <fstream>
#include <iostream>

using namespace std;

class BCC {
    int n;
    vector<vector<int>> G, bcs;
    vector<bool> isAncestor, isCritical;
    vector<int> depth, lowlink, wh;
    stack<int> st;

    void criticalNode(int x) {

    }
    
    void dfs(int x, int f) {
        depth[x] = depth[f] + 1;
        lowlink[x] = depth[x];
        st.push(x);
        isAncestor[x] = true;

        int sons = 0;
        
        for (auto y : G[x]) {
            if (y == f)
                continue;

            if (!depth[y]) {
                // DFS edge.
                dfs(y, x);
                ++sons;
                lowlink[x] = min(lowlink[x], lowlink[y]);

                if (lowlink[y] >= depth[x]) {
                    // x is Critical node.
                    isCritical[x] = true;

                    // Extract component rooted at x.
                    vector<int> bc;
                    int z;
                    do {
                        z = st.top();
                        wh[z] = bcs.size();
                        bc.push_back(z);
                        st.pop();
                    } while (z != y);
                    bc.push_back(x);
                    bcs.push_back(bc);
                }

                if (lowlink[y] > depth[x]) {
                    // Critical Edge.
                }
            } else if (isAncestor[y]) {
                // Back edge.
                lowlink[x] = min(lowlink[x], depth[y]);
            } else {
                // Forward edge.
            }
        }

        if (f == 0) {
            // Special case for root.
            if (sons > 1) {
                isCritical[x] = true;
            } else {
                isCritical[x] = false;
            }
        }
        
        isAncestor[x] = false;
    }
    
  public:
  
    BCC(vector<vector<int>>& G) : G(G) {
        n = G.size() - 1;
        
        depth = lowlink = wh = vector<int>(n+1);
        isAncestor = isCritical = vector<bool>(n+1);

        for (int i = 1; i <= n; ++i) {
            if (!depth[i]) {
                dfs(i, 0);
            }
        }
    }

    vector<vector<int>> getBcs() {
        return bcs;
    }
};

int main() {
    ifstream cin("biconex.in");
    ofstream cout("biconex.out");
    int n, m;

    cin >> n >> m;

    vector<vector<int>> G(n+1);

    for (int i = 1; i <= m; ++i) {
        int a, b;
        cin >> a >> b;
        G[a].push_back(b);
        G[b].push_back(a);
    }

    // cerr << "Here" << endl;
    BCC bcc(G);

    auto bcs = bcc.getBcs();

    cout << bcs.size() << "\n";
    for (auto& bc : bcs) {
        for (auto el : bc) {
            cout << el << " ";
        }
        cout << "\n";
    }
}
