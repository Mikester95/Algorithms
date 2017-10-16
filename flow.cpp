#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <climits>

using namespace std;

namespace flow {

typedef long long int64;

// Ford Fulkerson class for Max Flow. Currently built for sparse graphs.
// If you're not intereseted in flow lookups, delete the flow_lookup
// table to make the code faster.

struct Edge {
    int from, to, rev_ind;
    int64 flow, cap;
    Edge(int from, int to, int64 flow, int64 cap, int rev_ind) : from(from), to(to), 
    flow(flow), cap(cap), rev_ind(rev_ind) {}
};

class EKFlow {
  protected:
    int n, src, dest;
    int64 total_flow;
    vector<Edge> edge;
    vector<vector<int>> G;
    vector<unordered_map<int, int>> flow_lookup;

    vector<int> last_edge;

    virtual bool FindPath(bool multiple_paths = false) {
        for (int i = 1; i <= n; ++i) {
            last_edge[i] = -2;
        }
        queue<int> Q;
        last_edge[src] = -1;
        Q.push(src);

        while (!Q.empty()) {
            int x = Q.front();
            if (x == dest)
                return true;
            Q.pop();

            for (auto ind : G[x]) {
                auto& e = edge[ind];
                if (e.flow != e.cap && last_edge[e.to] == -2) {
                    Q.push(e.to);
                    last_edge[e.to] = ind;

                    if (!multiple_paths) {
                        if (e.to == dest)
                            return true;
                    } 
                }
            }
        }

        return false;
    }

    void UpdateLookup(int x, int y, int64 new_flow) {
        if (!flow_lookup[x].empty()) {
            flow_lookup[x][y] += new_flow;
        }
        if (!flow_lookup[y].empty()) {
            flow_lookup[y][x] -= new_flow;
        }
    }

    int64 FlowThrough() {
        int64 new_flow = LONG_LONG_MAX;

        for (int x = dest; x != src; x = edge[last_edge[x]].from) {
            auto& e = edge[last_edge[x]];
            new_flow = min(new_flow, e.cap - e.flow);
        }

        for (int x = dest; x != src; x = edge[last_edge[x]].from) {
            UpdateLookup(edge[last_edge[x]].from, x, new_flow);
            auto& e = edge[last_edge[x]];
            e.flow += new_flow;
            edge[e.rev_ind].flow -= new_flow;
        }

        total_flow += new_flow;
        return new_flow;
    }

    void BuildLookup(int x) {
        for (auto ind : G[x]) {
            flow_lookup[x][edge[ind].to] += edge[ind].flow;
        }
    }

  public:
    EKFlow(int n, int src, int dest) : n(n), src(src), dest(dest) {
        total_flow = 0;
        G.resize(n+1);
        flow_lookup.resize(n+1);
        last_edge.resize(n+1, -2);
    }

    bool RunUnit() {
        if (FindPath()) {
            for (int x = dest; x != src; x = edge[last_edge[x]].from) {
                auto& e = edge[last_edge[x]];
                e.flow += 1;
                edge[e.rev_ind].flow -= 1;
            }
            total_flow += 1;
            return true;
        }
        return false;
    }

    virtual int64 RunPath() {
        if (FindPath()) {
            return FlowThrough(); 
        }
        return 0;
    } 

    // Optimization of runPath function to fetch spanning tree of paths, instead
    // of single paths. Works considerably better when the flow network is wide
    // (like it is in Maximum Matching).
    // Separated from function that runs a single path because it's not compatible
    // with MinCostMaxFlow.
    int64 RunMultiplePaths() {
        if (FindPath(true)) {
            // cout << "Here\n";
            int64 new_flow = 0;
            for (auto edge_ind : G[dest]) {
                last_edge[dest] = edge[edge_ind].rev_ind;
                if (last_edge[edge[last_edge[dest]].from] == -2)
                    continue;
                new_flow += FlowThrough();
            }
            return new_flow;
        }
        return 0;
    }

    virtual void AddEdge(int x, int y, int64 cap, int64 flow=0) {
        while (x > n || y > n) {
            ++n;
            G.push_back(vector<int>());
            flow_lookup.push_back(unordered_map<int, int>());
            last_edge.push_back(-2);
        }

        int sz = edge.size();
        edge.push_back(Edge(x, y, flow, cap, sz+1));
        edge.push_back(Edge(y, x, -flow, 0, sz));

        G[x].push_back(sz);
        G[y].push_back(sz+1);
    }

    int64 GetFlow(int x, int y) {
        if (flow_lookup[x].empty()) {
            BuildLookup(x);
        }
        auto it = flow_lookup[x].find(y);
        if (it == flow_lookup[x].end()) {
            return 0;
        }
        return it->second;
    }

    int64 RunFlow() {
        for (; RunPath(););
        return total_flow;
    }

    int64 TotalFlow() {
        return total_flow;
    }
};

// TODO: Johnson Relabeling.
class EKCostFlow : public EKFlow {
  protected:
    int64 total_cost;
    vector<int64> cost;
    vector<int64> dist;    

    virtual bool FindPath(bool multiple_paths=false) {
        dist.resize(n+1);
        queue<int> Q;
        vector<bool> in_q(n+1);
        for (int i = 1; i <= n; ++i) {
            dist[i] = LONG_LONG_MAX;
        }
        dist[src] = 0;
        Q.push(src);
        in_q[src] = true;
        
        while (!Q.empty()) {
            int x = Q.front();

            for (auto ind : G[x]) {
                auto& e = edge[ind];
                if (e.flow != e.cap && dist[e.from] + cost[ind] < dist[e.to]) {
                    last_edge[e.to] = ind;
                    dist[e.to] = dist[e.from] + cost[ind];
                    if (!in_q[e.to]) {
                        Q.push(e.to);
                        in_q[e.to] = true;
                    }
                }
            }

            Q.pop();
            in_q[x] = false;
        }

        return dist[dest] != LONG_LONG_MAX;
    }

  public:
    EKCostFlow(int n, int src, int dest) : EKFlow(n, src, dest) {
        total_cost = 0;
    }

    virtual int64 RunPath() {
        int64 new_flow = EKFlow::RunPath();
        total_cost += new_flow * dist[dest];
        return new_flow; 
    }

    virtual void AddEdge(int x, int y, int cap, int64 cst, int64 flow=0) {
        EKFlow::AddEdge(x, y, cap, flow);
        cost.push_back(cst);
        cost.push_back(-cst);
    }

    int64 TotalCost() {
        return total_cost;
    }
};

}

int main() {
    flow::EKCostFlow ek(8, 1, 4);
    ek.AddEdge(1, 2, 1, 1);
    ek.AddEdge(1, 3, 1, 2);
    ek.AddEdge(2, 8, 1, 0);
    ek.AddEdge(2, 5, 1, 1);
    ek.AddEdge(5, 6, 1, 1);
    ek.AddEdge(6, 4, 1, 1);
    ek.AddEdge(3, 7, 1, 1);
    ek.AddEdge(7, 8, 1, 1);
    ek.AddEdge(8, 4, 1, 1);
    ek.RunFlow();
    // Should be 2 9.
    cout << ek.TotalFlow() << " " << ek.TotalCost() << "\n";
    // Should be 0 1.
    cout << ek.GetFlow(2, 8) << " " << ek.GetFlow(1, 2);
}