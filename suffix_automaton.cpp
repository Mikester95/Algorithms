#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

namespace strings {

struct Node {
    // max_len - length of the maximum substring represented by this state.
    // first_pos - smallest final position of an occurrence of the strings
    // representeed by this state.
    // prefix_leaf - whether this node contains a prefix(prefixes belong to different nodes).

    int max_len, first_pos;
    bool prefix_leaf;
    int suf;
    map<int, int> next;

    Node(int max_len, int first_pos, bool prefix_leaf) :
    max_len(max_len), first_pos(first_pos), prefix_leaf(prefix_leaf), suf(-1) {}

    inline int go(int c) {
        auto it = next.find(c);
        if (it == next.end())
            return -1;
        return it->second;
    }
};

class SuffixAutomaton {
    int last;
    vector<int> s;
    vector<Node> nodes;
    vector<int> topo;
    // paths_ending - number of distinct substrings ending in this set of terminal positions.
    // occ - number of occurrences of each string in this node.
    vector<long long> paths_starting, paths_ending, occ;

    int FollowSuf(int start, int c, int jump_over, int replacement) {
        for (; start != -1 && nodes[start].go(c) == jump_over; 
            start = nodes[start].suf) {
            nodes[start].next[c] = replacement;
        }
        return start;
    }
  
  public:
    void AddLetter(int c) {
        s.push_back(c);

        int travel = last;
        nodes.emplace_back(s.size(), s.size() - 1, true);
        int current = nodes.size() - 1;

        travel = FollowSuf(travel, c, -1, current);

        if (travel != -1) {
            int original = nodes[travel].go(c);
            if (nodes[original].max_len == nodes[travel].max_len + 1) {
                nodes[current].suf = original;
            } else {
                nodes.push_back(nodes[original]);
                int clone = nodes.size() - 1;
                nodes[clone].max_len = nodes[travel].max_len + 1;
                nodes[clone].prefix_leaf = false;
                nodes[original].suf = nodes[current].suf = clone;

                travel = FollowSuf(travel, c, original, clone);
            }
        } else {
            nodes[current].suf = 0;
        }

        last = current;
    }

    void Init() {
        nodes.push_back(Node(0, -1, false));
        last = 0;
    }

    void TopSort() {
        if (topo.size() == nodes.size())
            return;
        topo.resize(nodes.size());

        // NlogN. Can be reduced to O(N).
        for (int i = 0; i < nodes.size(); ++i) {
            topo[i] = i;
        }

        auto cmp = [&](int i, int j) {
            return nodes[i].max_len < nodes[j].max_len;
        };

        sort(topo.begin(), topo.end(), cmp);
    }

    void CountPathsStarting() {
        if (paths_starting.size() == nodes.size())
            return;
        paths_starting.resize(nodes.size());
        TopSort();

        for (int i = nodes.size() - 1; i >= 0; --i) {
            int x = topo[i];
            paths_starting[x] = 1;
            for (auto& edge : nodes[x].next) {
                paths_starting[x] += paths_starting[edge.second];
            }
        }   
    }

    void CountPathsEnding() {
        if (paths_ending.size() == nodes.size())
            return;
        paths_ending.resize(nodes.size());
        TopSort();

        paths_ending[0] = 1;
        for (int i = 0; i < nodes.size(); ++i) {
            int x = topo[i];
            for (auto& edge : nodes[x].next) {
                paths_ending[edge.second] += paths_ending[x];
            }
        }   
    }

    void CountOccurrences() {
        if (occ.size() == nodes.size())
            return;
        occ.resize(nodes.size());
        TopSort();

        for (int i = nodes.size()-1; i >= 0; --i) {
            int x = topo[i];
            if (nodes[x].prefix_leaf)
                occ[x] += 1;

            if (nodes[x].suf != -1)
                occ[nodes[x].suf] += occ[x];
        }
    }

    vector<long long> GetFrequencies() {
        CountOccurrences();
        CountPathsEnding();
        vector<long long> freq(*max_element(occ.begin(), occ.end()) + 1);

        for (int i = 0; i < nodes.size(); ++i) {
            if (i == 0)
                continue;
            freq[occ[i]] += paths_ending[i] * occ[i];
        }

        return freq;
    }

    string KthDistinctSubstring(long long k) {
        CountPathsStarting();

        int current = 0;
        string result;

        while (k > 0) {
            for (auto& edge : nodes[current].next) {
                long long extra = paths_starting[edge.second];
                if (extra < k) {
                    k -= extra;
                } else {
                    result += (char)(edge.first);
                    current = edge.second;
                    k -= 1;
                    break;
                }
            }
        }

        return result;
    }

    SuffixAutomaton() {
        Init();
    }

    SuffixAutomaton(const string& s) {
        Init();

        for (int i = 0; i < s.length(); ++i) {
            AddLetter(s[i]);
        }
    }
};
}

int main() {
    int tests;
    cin >> tests;

    for (;tests; --tests) {     
        string s;
        cin >> s;
        strings::SuffixAutomaton automaton(s);

        auto freq = automaton.GetFrequencies();

        for (int i = freq.size()-2; i >= 0; --i) {
            freq[i] += freq[i+1];
        }

        int q;
        cin >> q;

        for (int i = 1; i <= q; ++i) {
            int f;
            cin >> f;
            if (f >= freq.size())
                cout << 0 << "\n";
            else cout << freq[f] << "\n";
        }
    }
}