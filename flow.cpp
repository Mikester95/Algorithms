#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <climits>
#include <functional>

using namespace std;

namespace flow {

typedef long long int64;

// TO TEST(Dinic, Hopcroft Karp).
// Classes for solving Max Flow and related problems. 
// They use adjacency lists to work fast for sparse graphs.

struct Edge {
    int from, to;
    int64 flow, cap;
    Edge(int from, int to, int64 flow, int64 cap) : from(from), to(to), 
    flow(flow), cap(cap) {}
};

// Ford Fulkerson interface for most of the max flow algorithms.
// If you're not intereseted in flow lookups, delete the flow_lookup table to make the code faster.
class FordFulkerson{
  protected:
    int n, src, dest;
    int64 total_flow;
    vector<Edge> edges;
    vector<vector<int>> neighbours;
    vector<unordered_map<int, int>> flow_lookup;

    // Index of reverse edge of a given edge index.
    int rev_ind(int i) {
    	return i ^ 1;
    }

    // Builds a lookup table for fast queries of total between x and one of his neighbours.
    void BuildLookup(int x) {
        for (auto ind : neighbours[x]) {
            flow_lookup[x][edges[ind].to] += edges[ind].flow;
        }
    }

    // Updates lookup table of x and y if they have been built.
    void UpdateLookup(int x, int y, int64 new_flow) {
        if (!flow_lookup[x].empty()) {
            flow_lookup[x][y] += new_flow;
        }
        if (!flow_lookup[y].empty()) {
            flow_lookup[y][x] -= new_flow;
        }
    }

    // Run flow through edge ind.
    void AddFlow(int ind, int64 flow) {
        UpdateLookup(edges[ind].from, edges[ind].to, flow);
        edges[ind].flow += flow;
        edges[rev_ind(ind)].flow -= flow;
    }

    // How much empty room does edge ind have?
    inline int64 room(int ind) {
    	return edges[ind].cap - edges[ind].flow;
    }

    inline int from(int ind) {
    	return edges[ind].from;
    }

    inline int to(int ind) {
    	return edges[ind].to;
    }

    // Searches for some sort of path or multitude of augmenting paths from source to sink.
    virtual bool FindPath() = 0;
  
  public:
    FordFulkerson(int n, int src, int dest) : n(n), src(src), dest(dest) {
        total_flow = 0;
        neighbours.resize(n+1);
        flow_lookup.resize(n+1);
    }

    // Get the total amount of flow between x and y.
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

    // Pumps flow through the network using FindPath.
    virtual int64 RunFlow() = 0;

    // Adds an edge to the graph.
    virtual void AddEdge(int x, int y, int64 cap, int64 flow = 0) {
        while (x > n || y > n) {
            ++n;
            neighbours.push_back(vector<int>());
            flow_lookup.push_back(unordered_map<int, int>());
        }

        int sz = edges.size();
        edges.push_back(Edge(x, y, flow, cap));
        edges.push_back(Edge(y, x, -flow, 0));
        neighbours[x].push_back(sz);
        neighbours[y].push_back(sz + 1);
    }

    // Runs the maximum amount of flow through the network.
    int64 MaxFlow() {
        for (;RunFlow(););
        return total_flow;
    }

    // Returns the current amount of flow.
    int64 TotalFlow() {
    	return total_flow;
    }

    // Returns the minimum src-dest cut in the form of a binary vector.
    // 1 means node is on src side, 0 means node is on dest side.
    vector<bool> GetCut() {
    	vector<bool> visited(n + 1);

    	function<void(int)> DFS = [&](int x) {
	  		visited[x] = true;
	  		for (auto ind: neighbours[x]) {
	  			if (room(ind)) {
	  				DFS(to(ind));
	  			}
	  		}
	  	};

    	DFS(src);
    	return visited;
    }

    // Returns the graph's edges.
    const vector<Edge>& GetEdges() {
    	return edges;
    } 
};

// Edmonds Karp algorithm for finding max flow. Works in O(Flow * M) or O(N * M^2).
class EdmondsKarp : public FordFulkerson {
  protected:
    vector<int> last_edge;

    virtual bool FindPath() {
    	last_edge.resize(n + 1);
        queue<int> Q;
        for (int i = 1; i <= n; ++i) {
            last_edge[i] = -2;
        }
        last_edge[src] = -1;
        Q.push(src);

        while (!Q.empty()) {
            int x = Q.front();
            Q.pop();

            for (auto ind : neighbours[x]) {
            	int y = to(ind);
                if (room(ind) && last_edge[y] == -2) {
                    Q.push(y);
                    last_edge[y] = ind;

                    if (y == dest)
                        return true;
                }
            }
        }

        return false;
    }

  public:
  	EdmondsKarp(int n, int src, int dest) : FordFulkerson(n, src, dest) {}

    virtual bool RunUnit() {
        if (FindPath()) {
            for (int x = dest; x != src; x = from(last_edge[x])) {
            	AddFlow(last_edge[x], 1);
            }
            total_flow += 1;
            return true;
        }
        return false;
    }

    virtual int64 RunFlow() {
        if (FindPath()) {
	        int64 new_flow = LONG_LONG_MAX;

	        for (int x = dest; x != src; x = from(last_edge[x])) {
	        	new_flow = min(new_flow, room(last_edge[x]));
	        }

	        for (int x = dest; x != src; x = from(last_edge[x])) {
	        	AddFlow(last_edge[x], new_flow);
	        }

	        total_flow += new_flow;
	        return new_flow;
        }
        return 0;
    } 
};

// Dinic algorithm for finding max flow. It works in O(N^2 * M)
// or in O(M * min(M^1.5, N^0.66)) for unit capacity networks.
class Dinic : public FordFulkerson {
  protected:
  	vector<int> dist, adj_list_pointers;

  	virtual bool FindPath() {
  		dist.resize(n + 1);
  		queue<int> Q;
  		for (int i = 1; i <= n; ++i) {
  			dist[i] = INT_MAX;
  		}
  		dist[src] = 0;
  		Q.push(src);

  		while (!Q.empty()) {
  			int x = Q.front();
  			Q.pop();

  			for (auto ind : neighbours[x]) {
  				int y = to(ind);
  				if (room(ind) && dist[y] == INT_MAX) {
  					Q.push(y);
  					dist[y] = dist[x] + 1;
  				}
  			}
  		}

  		return dist[dest] != INT_MAX;
  	}

  	int64 Backtrack(int x, int64 available_flow) {
  		if (x == dest) {
  			return available_flow;
  		}

  		int64 run_flow = 0;
  		for (; adj_list_pointers[x] < neighbours[x].size(); 
  			++adj_list_pointers[x]) {
  			int i = adj_list_pointers[x];
  			int y = to(neighbours[x][i]);
  			if (dist[x] + 1 != dist[y])
  				continue;

  			int64 new_flow = Backtrack(y, min(available_flow, room(neighbours[x][i])));
  			available_flow -= new_flow;
  			run_flow += new_flow;
  			AddFlow(neighbours[x][i], new_flow);

  			if (!available_flow)
  				break;
  		}

  		return run_flow;
  	}

  public:
  	Dinic(int n, int src, int dest) : FordFulkerson(n, src, dest) {}

  	virtual int64 RunFlow() {
  		if (FindPath()) {
	  		adj_list_pointers.resize(n + 1);
	  		for (int i = 1; i <= n; ++i) {
	  			adj_list_pointers[i] = 0;
	  		}
	  		int64 new_flow = Backtrack(src, LONG_LONG_MAX);
	  		total_flow += new_flow;
	  		return new_flow;
  		}
  		return 0;
  	}
};

// Edmonds-Karp algorithm for min-cost flow. 
// Uses Bellman-Ford to find the first shortest path. After that,
// it uses Johnson potentials to make the edge costs positive in order to use Dijkstra.
// Worst-case complexity is O(N * M + Flow * M * log(M)).
// TODO: Negative Cycle canceling if initial graph has negative cycles.
class EdmondsKarpCost : public EdmondsKarp {
  protected:
    int64 total_cost;
    vector<int64> cost;
    vector<int64> dist;    

    bool BellmanFord() {
        dist.resize(n + 1);
        last_edge.resize(n + 1);
        queue<int> Q;
        vector<bool> in_q(n + 1);
        for (int i = 1; i <= n; ++i) {
            dist[i] = LONG_LONG_MAX;
        }
        dist[src] = 0;
        Q.push(src);
        in_q[src] = true;
        
        while (!Q.empty()) {
            int x = Q.front();
            for (auto ind : neighbours[x]) {
                int y = to(ind);
                if (room(ind) && dist[x] + cost[ind] < dist[y]) {
                    last_edge[y] = ind;
                    dist[y] = dist[x] + cost[ind];
                    if (!in_q[y]) {
                        Q.push(y);
                        in_q[y] = true;
                    }
                }
            }

            Q.pop();
            in_q[x] = false;
        }
        return dist[dest] != LONG_LONG_MAX;
    }    

    bool Dijkstra() {
        last_edge.resize(n + 1);
        vector<int64> new_dist(n + 1); 
        vector<bool> seen(n + 1);
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> H;
        for (int i = 1; i <= n; ++i) {
            new_dist[i] = LONG_LONG_MAX;
        }
        new_dist[src] = 0;
        H.push({0, src});
        
        while (!H.empty()) {
            int x = H.top().second;
            H.pop();

            if (seen[x])
                continue;
            seen[x] = true;

            for (auto ind : neighbours[x]) {
                int y = to(ind);
                int64 potential_cost = cost[ind] + dist[x] - dist[y];
                if (room(ind) && new_dist[x] + potential_cost < new_dist[y]) {
                    last_edge[y] = ind;
                    new_dist[y] = new_dist[x] + potential_cost;
                    H.push({new_dist[y], y});
                }
            }
        }

        // Remove the potentials from new_dist.
        for (int i = 1; i <= n; ++i) {
            if (new_dist[i] != LONG_LONG_MAX)
                new_dist[i] += dist[i];
        }
        dist = new_dist;

        return dist[dest] != LONG_LONG_MAX;
    }

    virtual bool FindPath() {
        if (dist.size() != n + 1) {
            return BellmanFord();
        } else {
            return Dijkstra();
        }
    }

  public:
    EdmondsKarpCost(int n, int src, int dest) : EdmondsKarp(n, src, dest) {
        total_cost = 0;
    }

    virtual bool RunUnit() {
    	bool run = EdmondsKarp::RunUnit();
    	total_cost += run * dist[dest];
    	return run;
    }

    virtual int64 RunFlow() {
        int64 new_flow = EdmondsKarp::RunFlow();
        total_cost += new_flow * dist[dest];
        return new_flow; 
    }

    virtual void AddEdge(int x, int y, int cap, int64 cst, int64 flow = 0) {
        EdmondsKarp::AddEdge(x, y, cap, flow);
        cost.push_back(cst);
        cost.push_back(-cst);
    }

    int64 TotalCost() {
        return total_cost;
    }
};

// HopcroftKarp class to find MaxMatching/MinVertexCover in Bipartite Graph.
// It uses Dinic max flow behind the scenes, achieving M * sqrt(N) complexity.
class HopcroftKarp {
	int nodes;
	vector<bool> is_left_node;
	FordFulkerson* flow_solver;

  public:
	HopcroftKarp(const vector<vector<int>>& graph) {
        nodes = graph.size() - 1;
        vector<bool> visited(nodes + 1);
        is_left_node.resize(nodes + 1);
        flow_solver = new Dinic(nodes, 1, 2);

        function<void(int, bool)> SplitDFS = [&](int x, bool side) {
	        visited[x] = true;
	        is_left_node[x] = side;

	        for (auto& y : graph[x]) {
	            if (!visited[y]) {
	                SplitDFS(y, !side);
	            } else if (is_left_node[y] != !is_left_node[x]) {
	                cerr << "Graph is not bipartite!!!" << endl;
	            }
	        }
        };

        for (int i = 1; i <= nodes; ++i) {
            if (!visited[i]) {
                SplitDFS(i, true);
            }
        }

        for (int x = 1; x <= nodes; ++x) {
        	if (is_left_node[x]) {
        		for (auto& y : graph[x]) {
        			flow_solver->AddEdge(x + 2, y + 2, 1);
        		}
        	}
        }

        for (int i = 1; i <= nodes; ++i) {
        	if (is_left_node[i]) {
        		flow_solver->AddEdge(1, i + 2, 1);
        	} else {
        		flow_solver->AddEdge(i + 2, 2, 1);
        	}
        }
	}

	~HopcroftKarp() {
		delete flow_solver;
	}

	int MaxMatching() {
        return flow_solver->MaxFlow();
    }
 
    vector<pair<int,int>> GetMatching() {
        vector<pair<int,int>> sol;
        sol.reserve(MaxMatching());
        for (const auto& edge: flow_solver->GetEdges()) {
        	if (edge.flow == 1 && edge.from != 1 && edge.to != 2 && is_left_node[edge.from - 2]) {
        		sol.push_back({edge.from - 2, edge.to - 2});
        	}
        }
        return sol;
    }

    int MinVertexCover() {
        return MaxMatching();  
    }

    int MaxIndependentSet() {
        return nodes - MinVertexCover();
    }

    vector<int> GetVertexCover() {
        vector<int> sol;
        sol.reserve(MinVertexCover());
       	auto cut = flow_solver->GetCut();

        for (int i = 1; i <= nodes; ++i) {
            if (is_left_node[i] && !cut[i + 2] ||
            	!is_left_node[i] && cut[i + 2])
                sol.push_back(i);
        }
        return sol;
    }

    vector<int> GetIndependentSet() {
        vector<int> sol;
        sol.reserve(MaxIndependentSet());
        auto cut = flow_solver->GetCut();

        for (int i = 1; i <= nodes; ++i) {
            if (is_left_node[i] && cut[i + 2] ||
            	!is_left_node[i] && !cut[i + 2])
                sol.push_back(i);
        }
        return sol;
    }

};

}

int main() {
    flow::EdmondsKarpCost ekc(8, 1, 4);
    ekc.AddEdge(1, 2, 1, 1);
    ekc.AddEdge(1, 3, 1, 2);
    ekc.AddEdge(2, 8, 1, 0);
    ekc.AddEdge(2, 5, 1, 1);
    ekc.AddEdge(5, 6, 1, 1);
    ekc.AddEdge(6, 4, 1, 1);
    ekc.AddEdge(3, 7, 1, 1);
    ekc.AddEdge(7, 8, 1, 1);
    ekc.AddEdge(8, 4, 1, 1);
    ekc.MaxFlow();
    // Should be 2 9.
    cout << ekc.TotalFlow() << " " << ekc.TotalCost() << "\n";
    // Should be 0 1.
    cout << ekc.GetFlow(2, 8) << " " << ekc.GetFlow(1, 2) << "\n";

    flow::Dinic ek(8, 1, 4);
    ek.AddEdge(1, 2, 1);
    ek.AddEdge(1, 3, 1);
    ek.AddEdge(2, 8, 1);
    ek.AddEdge(2, 5, 1);
    ek.AddEdge(5, 6, 1);
    ek.AddEdge(6, 4, 1);
    ek.AddEdge(3, 7, 1);
    ek.AddEdge(7, 8, 1);
    ek.AddEdge(8, 4, 1);
    ek.MaxFlow();

    cout << ek.TotalFlow() << "\n";
    cout << ek.GetFlow(2, 8) << "\n";
}