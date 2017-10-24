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

// Solves an instance of 2SAT, that you can add variables and restrictions to dynamically.
// Variables have a public and private representation.
// Publically, variables are indexed 1 through n, and their negation as -1 to -n.
// Internally, variable i is stored as i*2 - 1, and its negation as i*2.
class TwoSAT {
    // Number of variables.
    int n;
    // Private representation of graph.
    vector<vector<int>> G;

    // Public representation to private representation.
    int ind(int x) {
        return x > 0 ? ((x << 1) - 1) : (((-x) << 1));
    }

    // Oposite of private representation.
    int op(int x) {
        return (x&1) ? x + 1 : x - 1;
    }

    // Resize structure to fit at least n variables.
    void Resize(int n) {
        if (n > this->n) {
            this->n = n;
            G.resize(max(ind(n), ind(-n)) + 1);
        }
    }

    // Adds an edge in the private representation.
    void AddEdge(int x, int y) {
        G[x].push_back(y);
        G[op(y)].push_back(op(x));
    }

  public:
    // Operations with the public representations.
    void Implication(int x, int y) {
        Resize(max(abs(x), abs(y)));
        AddEdge(ind(x), ind(y));
    }

    void Disjunction(int x, int y) {
        Resize(max(abs(x), abs(y)));
        AddEdge(ind(-x), ind(y));
    }

    void SetToZero(int x) {
        Resize(abs(x));
        G[ind(x)].push_back(op(ind(x)));
    }

    void SetToOne(int x) {
        Resize(abs(x));
        G[op(ind(x))].push_back(ind(x));
    }

    TwoSAT(int n=0) : n(n) {
        G.resize(2*n + 3);
    }

    // Solves the current instance using Tarjan's SCC algorithm,
    // and returns the value attributions, or an empty vector if there is no solution.
    vector<bool> Solve() {
        TarjanSCC sccSolver(G);
        auto wh = sccSolver.GetWh();

        for (int i = 1; i <= n; ++i) {
            if (wh[ind(i)] == wh[op(ind(i))])
                return {};
        }

        vector<bool> sol(n+1);
        for (int i = 1; i <= n; ++i) {
            sol[i] = (wh[ind(i)] > wh[op(ind(i))] ? 0 : 1);
        }

        return sol;
    }
};

int main() {
    freopen("2sat.in", "r", stdin);
    freopen("2sat.out", "w", stdout);
    int n, m;
    cin >> n >> m;

    TwoSAT solver(n);

    for (int i = 1; i <= m; ++i) {
        int a, b;
        cin >> a >> b;

        solver.Disjunction(a, b);
    }

    auto sol = solver.Solve();

    if (sol.empty()) {
        cout << -1 << "\n";
    } else {
        for (int i = 1; i <= n; ++i) {
            cout << sol[i] << " ";
        }
    }
}