
// Constructs the dual of a planar graph on N vertices with specified edges.
// addOuterFace=true has not been thouroughly tested.
// You have to modify the lambda function for sorting the neighbours of a vertex in a circular order.
// Returns pair of (lists of indices of points sorrounding each face in circular order,
//                  list of ordered edges between faces. Ordered means (x, y) will also appear as (y, x))
// Two faces are connected by an edge in the dual if they are neighbouring in the primal.
pair<vector<vector<int>>, vector<pair<int,int>>> buildDual(
    int n, vector<pair<int,int>>& edges, bool addOuterFace=true) {

    vector<vector<int>> faces;
    vector<pair<int,int>> faceEdges;
    vector<pair<int,int>> edgePos(edges.size());
    vector<vector<pair<int,int>>> graph(n);
    vector<vector<bool>> visited(n);
    vector<vector<int>> whichFace(n);

    for (int i = 0; i < edges.size(); ++i) {
        graph[edges[i].first].push_back({edges[i].second, i});
        graph[edges[i].second].push_back({edges[i].first, i});
    }

    for(int node = 0; node < n; ++node) {

        // Rewrite this function.
        auto cmp = [&] (pair<int,int> a, pair<int,int> b) {
            if (a.first < node)
                a.first += n;
            if (b.first < node)
                b.first += n;
            return a.first < b.first;
        };
        sort(graph[node].begin(), graph[node].end(), cmp);
        for (int i = 0; i < graph[node].size(); ++i) {
            int edgeIndex = graph[node][i].second;
            if (edges[edgeIndex].first == node)
                edgePos[edgeIndex].first = i;
            else edgePos[edgeIndex].second = i;
        }
        visited[node].resize(graph[node].size());
        whichFace[node].resize(graph[node].size());
    }


    auto getCorrespondent = [&](int x, int ind) {
        int edgeIndex = graph[x][ind].second;
        if (x == edges[edgeIndex].first) {
            return make_pair(edges[edgeIndex].second, edgePos[edgeIndex].second);
        } else {
            return make_pair(edges[edgeIndex].first, edgePos[edgeIndex].first);
        }
    };

    for (int node = 0; node < n; ++node) {
        for (int i = 0; i < graph[node].size(); ++i) {
            if (i != graph[node].size() - 1 || addOuterFace) {
                if (!visited[node][i]) {
                    vector<int> faceVertices;

                    int x = node;
                    int ind = i;

                    while (!visited[x][ind]) {
                        faceVertices.push_back(x);
                        visited[x][ind] = true;
                        whichFace[x][ind] = faces.size();
                        tie(x, ind) = getCorrespondent(x, ind);
                        ind = (ind - 1 + graph[x].size()) % graph[x].size();
                    }

                    faces.push_back(faceVertices);
                }

                if (i != 0) {
                    faceEdges.push_back({whichFace[node][i], whichFace[node][i-1]});
                }
            }
        }
        if (addOuterFace) {
            faceEdges.push_back({whichFace[node][0], whichFace[node][graph[node].size()-1]});
        }
    }

    return {faces, faceEdges};
}
