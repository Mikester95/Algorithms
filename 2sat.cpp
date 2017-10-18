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

class TwoSAT {
    // Number of variables.
    int n;
    // Private representation of graph.
    vector<vector<int>> G;
    // Index of zero in private representation.
    const int zero;

    // Public representation to private representation.
    int ind(int x) {
        return x > 0 ? ((x << 1) + 1) : (((-x) << 1) + 2);
    }

    // Oposite of private representation.
    int op(int x) {
        return (x&1) ? x + 1 : x - 1;
    }

    // Resize structure to fit n variables.
    void Resize(int n) {
        if (n > this->n) {
            this->n = n;
            G.resize(max(ind(n), ind(-n)) + 1);
        }
    }

    void AddEdge(int x, int y) {
        G[x].push_back(y);
        G[op(y)].push_back(op(x));
    }

  public:
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
        AddEdge(ind(x), zero);
    }

    void SetToOne(int x) {
        Resize(abs(x));
        AddEdge(op(zero), ind(x));
    }

    TwoSAT(int n=0) : n(n), zero(1) {
        G.resize(2*n + 3);
        // Edge from zero to op(zero) ensures its value will be zero.
        AddEdge(zero, op(zero));
    }

    vector<bool> Solve() {
        TarjanSCC sccSolver(G);
        auto wh = sccSolver.GetWh();

        if (wh[zero] == wh[op(zero)])
            return {};

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