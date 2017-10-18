#include <vector>
#include <stack>
#include <fstream>
#include <iostream>

using namespace std;

class TarjanBCC {
    int n;
    vector<vector<int>> G, bccs;
    vector<bool> isAncestor, isCritical;
    vector<int> depth, lowlink, wh;
    stack<int> st;
    
    void Dfs(int x, int f) {
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
                Dfs(y, x);
                ++sons;
                lowlink[x] = min(lowlink[x], lowlink[y]);

                if (lowlink[y] >= depth[x]) {
                    // x is Critical node.
                    isCritical[x] = true;

                    // Extract component rooted at x.
                    vector<int> bcc;
                    int z;
                    do {
                        z = st.top();
                        wh[z] = bccs.size();
                        bcc.push_back(z);
                        st.pop();
                    } while (z != y);
                    bcc.push_back(x);
                    bccs.push_back(bcc);
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
  
    TarjanBCC(vector<vector<int>>& G) : G(G) {
        n = G.size() - 1;
        
        depth = lowlink = wh = vector<int>(n+1);
        isAncestor = isCritical = vector<bool>(n+1);

        for (int i = 1; i <= n; ++i) {
            if (!depth[i]) {
                Dfs(i, 0);
            }
        }
    }

    vector<vector<int>> GetBccs() {
        return bccs;
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
    TarjanBCC bcc(G);

    auto bcs = bcc.GetBccs();

    cout << bcs.size() << "\n";
    for (auto& bc : bcs) {
        for (auto el : bc) {
            cout << el << " ";
        }
        cout << "\n";
    }
}
