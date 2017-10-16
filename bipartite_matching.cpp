#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class HKMatcher {
  public:
    int matchSize, nodes;
    vector<vector<int>> G;
    vector<int> p;
    vector<bool> visited, selected, isLeft;

    void splitDfs(int x, int side) {
        visited[x] = true;
        isLeft[x] = side;

        for (auto& y : G[x]) {
            if (!visited[y]) {
                splitDfs(y, 1 - side);
            } else if (isLeft[y] != !isLeft[x]) {
                cerr << "Graph is not bipartite!!!" << endl;
            }
        }
    }

    // rightNodes == -1 means implicit bipartite graph has been given
    // Otherwise rightNodes should be number of nodes on the right side.
    HKMatcher(const vector<vector<int>>& G, int rightNodes=-1) : G(G) {
        if (rightNodes > -1) {
            int leftNodes = this->G.size() - 1;
            for (int i = 1; i <= leftNodes; ++i) {
                for (auto& y : this->G[i]) {
                    y += leftNodes;
                }
            }
            nodes = leftNodes + rightNodes;
            isLeft.resize(nodes + 1);
            for (int i = 1; i <= leftNodes; ++i) {
                isLeft[i] = true;
            }
        } else {
            nodes = this->G.size() - 1;
            visited.resize(nodes + 1);
            isLeft.resize(nodes + 1);
            for (int i = 1; i <= nodes; ++i) {
                if (!visited[i]) {
                    splitDfs(i, 1);
                }
            }
            for (int i = 1; i <= nodes; ++i) {
                visited[i] = 0;
            }
        }
    }
 
    bool pairDfs(int x) {
        if (visited[x])
            return false;
        visited[x] = true;
 
        for (auto y : G[x]) {
            if (!p[y]) {
                p[x] = y;
                p[y] = x;
                return true;
            }
        }
 
        for (auto y : G[x]) {
            if (pairDfs(p[y])) {
                p[x] = y;
                p[y] = x;
                return true;
            }
        }
         
        return false;
    }
 
    int maxMatching() {
        matchSize = 0;
        p.resize(nodes + 1);
        visited.resize(nodes + 1);
        
        bool done = false;
        
        while (!done) {
            done = true;
            for (int i = 1; i <= nodes; ++i) {
                visited[i] = 0;
            }
            for (int i = 1; i <= nodes; ++i) {
                if (isLeft[i] && !p[i]) {
                    if (pairDfs(i)) {
                        done = false;
                        ++matchSize;
                    }
                }
            }
        }
        return matchSize;
    }
 
    vector<pair<int,int>> getMatching() {
        vector<pair<int,int>> sol;
        sol.reserve(matchSize);
        for (int i = 1; i <= nodes; ++i) {
            if (isLeft[i] && p[i]) {
                sol.push_back({i, p[i]});
            }
        }
        return sol;
    }

    void coverDfs(int x) {
        for (auto y : G[x]) {
            if (!selected[y]) {
                selected[y] = true;
                selected[p[y]] = false;
                coverDfs(p[y]);
            }
        }
    }

    int minVertexCover() {
        maxMatching();
        selected.resize(nodes + 1);

        for (int i = 1; i <= nodes; ++i) {
            if (isLeft[i] && p[i])
                selected[i] = true;
        }

        for (int i = 1; i <= nodes; ++i) {
            if (isLeft[i] && !p[i]) {
                coverDfs(i);
            }
        }

        return matchSize;
    }

    int maxIndependentSet() {
        return nodes - minVertexCover();
    }

    vector<int> getVertexCover() {
        vector<int> sol;
        sol.reserve(matchSize);
        for (int i = 1; i <= nodes; ++i) {
            if (selected[i])
                sol.push_back(i);
        }
        return sol;
    }

    vector<int> getIndependentSet() {
        vector<int> sol;
        sol.reserve(nodes - matchSize);
        for (int i = 1; i <= nodes; ++i) {
            if (!selected[i]) {
                sol.push_back(i);
            }
        }
        return sol;
    }
};

int main() {
    ifstream fin("felinare.in");
    ofstream fout("felinare.out");

    int n, m;
    fin >> n >> m;
    vector<vector<int>> G(2*n+1);

    for (int i = 1; i <= m; ++i) {
        int x, y;
        fin >> x >> y;
        G[x].push_back(y+n);
        G[y+n].push_back(x);
    }

    HKMatcher matcher(G);
    fout << matcher.maxIndependentSet() << "\n";

    for (int i = 1; i <= n; ++i) {
        if (matcher.selected[i] && matcher.selected[i + n]) {
            fout << 0;
        } else if (matcher.selected[i]) {
            fout << 2;
        } else if (matcher.selected[i + n]) {
            fout << 1;
        } else {
            fout << 3;
        }
        fout << "\n";
    }
}