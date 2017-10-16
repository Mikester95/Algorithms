#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

namespace trie {

struct Node {
    int count, sub;
    map<int, int> next;

    Node(int count, int sub) :
    count(count), sub(sub) {}

    inline int go(int c) {
        auto it = next.find(c);
        if (it == next.end())
            return -1;
        return it->second;
    }
};

template <typename T>
class Trie {
    T s, k;
    vector<Node> nodes;
    vector<int> roots;
    bool persistent;

    void reset(T& s) {
        s = 0;
    }

    void add_ch(T& s, int c, int depth) {
        s = (s<<1) + c;
    }

    int get_ch(const T& s, int depth) {
        return (s>>(31-depth))&1;
    }

    bool is_finished(const T& s, int depth) {
        return depth == 32;
    }

    void InsertRec(int node, int depth) {
        nodes[node].sub += 1;
        if (is_finished(s, depth)) {
            nodes[node].count += 1;
            return;
        }

        int c = get_ch(s, depth);
        int follow = nodes[node].go(c);
        if (follow == -1) {
            nodes.push_back(Node(0, 0));
            nodes[node].next[c] = follow = nodes.size() - 1;
        } else if (persistent) {
            nodes.push_back(nodes[follow]);
            nodes[node].next[c] = follow = nodes.size() - 1;
        }

        InsertRec(follow, depth + 1);
    }

    void MaxRec(int node, int depth) {
        // cerr << "Max " << k << " at depth " << depth << " and sub " << nodes[node].sub << "\n";
        if (nodes[node].count &&
            nodes[node].sub == nodes[node].count) {
            return;
        }

        pair<int, int> it;
        if (get_ch(k, depth) == 0)
            it = *nodes[node].next.rbegin();
        else it = *nodes[node].next.begin();
        add_ch(s, it.first, depth);

        MaxRec(it.second, depth + 1);
    }

    void MinRec(int node, int depth) {
        // cerr << "Min " << k << " at depth " << depth << " and sub " << nodes[node].sub << "\n";
        if (nodes[node].count) {
            return;
        }
        
        pair<int, int> it;
        if (get_ch(k, depth) == 0)
            it = *nodes[node].next.begin();
        else it = *nodes[node].next.rbegin();
        add_ch(s, it.first, depth);

        MinRec(it.second, depth + 1);
    }

  public:

    int Insert(const T& s, int root=0) {
        this->s = s;
        if (persistent) {
            nodes.push_back(nodes[roots[root]]);
            roots.push_back(nodes.size()-1);
            root = roots.size() - 1;
        }
        InsertRec(roots[root], 0);
        return root;
    }

    Trie(bool persistent=false) : persistent(persistent) {
        nodes.push_back(Node(0, 0));
        roots.push_back(0);
    }

    T Max(T k, int root=0) {
        this->k = k;
        reset(s);

        if (nodes[roots[root]].sub == 0)
            return k;
        
        MaxRec(roots[root], 0);
        return s ^ k;
    }

    T Min(T k, int root=0) {
        this->k = k;
        reset(s);

        if (nodes[roots[root]].sub == 0)
            return k;

        MinRec(roots[root], 0);
        return s ^ k;
    }
};
}