#include <iostream>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

namespace pal {

// Returns vector of 2N + 1 numbers, signifying the largest palindromes
// centered in each of the 2N + 1 possible centers.
vector<int> Manacher(const string& s, char delim=0) {
    int n = s.length();
    vector<char> ext(2 * n + 1);
    vector<int> pal(2 * n + 1);

    ext[0] = delim;
    for (int i = 0; i < n; ++i) {
        ext[2 * i + 1] = s[i];
        ext[2 * i + 2] = delim;
    }

    int maxi = 0;

    for (int i = 0; i < ext.size(); ++i) {
        int current = min(pal[2*maxi - i], maxi - i + pal[maxi]);
        while (i + current < ext.size() && i - current >= 0 &&
            ext[i + current] == ext[i - current])
            ++current;
        pal[i] = current;

        if (pal[i] + i - 1 > pal[maxi] + maxi - 1)
            maxi = i;
    }

    for (int i = 0; i < pal.size(); ++i) {
        pal[i] >>= 1;
    }

    return pal;
}

struct Node {
    // len - depth of node wrt transitions. Also length of palindrome represented by this node.
    // depth - depth of node wrt suffix links. Also number of palindromes that 
    // are suffixes of this palindrome. For roots it is 0.
    // count - number of positions that have this palindrome as their greatest suffix palindrome.
    int len, depth, count, suf;

    map<int, int> next;

    Node(int len, int depth, int count, int suf) : len(len), 
    depth(depth), count(count), suf(suf) {}

    inline int go(int c) {
        auto it = next.find(c);
        if (it == next.end())
            return -1;
        return it->second;
    }
};
 
class PalTree {
    int uneven_root, even_root, last;
    vector<int> s;
    vector<Node> nodes;

    // occ - counts occurrences of each palindrome in the string.
    vector<long long> occ;

    int FollowSuf(int start, int pos) {
        while(pos - nodes[start].len - 1 < 0 || 
            s[pos] != s[pos - nodes[start].len - 1])
            start = nodes[start].suf;
        return start;
    }

 public:

    // Adds a new letter. Also returns number of distinct 
    // palindromes ending at this new position.
    int AddLetter(int c) {
        s.push_back(c);
        int travel = FollowSuf(last, s.size() - 1);
        int current;
        if (nodes[travel].go(c) == -1) {
            int suf;
            if (travel == uneven_root) {
               suf = even_root;
            } else {
               suf = nodes[FollowSuf(nodes[travel].suf, s.size() - 1)].go(c);
            }
            nodes.emplace_back(nodes[travel].len + 2, nodes[suf].depth + 1, 1, suf);
            current = nodes.size() - 1;
            nodes[travel].next[c] = current;
        } else {
            current = nodes[travel].go(c);
            nodes[current].count += 1;
        }
        last = current;
        return nodes[current].depth;
    } 

    void Init() {
        uneven_root = 0;
        nodes.emplace_back(-1, 0, 0, -1);
        even_root = 1;
        nodes.emplace_back(0, 0, 0, 0);
        last = even_root;
    }

    void CountOccurrences() {
        if (occ.size() == nodes.size()) {
            return;
        }
        occ.resize(nodes.size());

        for (int i = nodes.size()-1; i >= 0; --i) {
            if (i == 0)
                continue;
            occ[i] += nodes[i].count;
            occ[nodes[i].suf] += occ[i];
        }
    }

    long long Pal2() {
        CountOccurrences();
        long long ans = 0;

        for (int i = 0; i < nodes.size(); ++i) {
            if (i == 0)
                continue;
            ans += 1LL * occ[i] * occ[i] * nodes[i].len;
        }
        return ans;
    }
 
    PalTree() {
        Init();
    }

    PalTree(const string& s) {
        Init();

        for (int i = 0; i < s.length(); ++i) {
            AddLetter(s[i]);
        }
    }
};
}

int main() {
    ifstream fin("pscpld.in");
    ofstream fout("pscpld.out");

    string s;
    fin >> s;

    auto pal = pal::Manacher(s);

    long long ans = 0;
    for(auto el : pal) {
        ans += el;
    }

    fout << ans;
}