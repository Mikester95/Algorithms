#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace strings {

struct DoubleChunk {
    int x[2], ind;

    DoubleChunk() {}
    DoubleChunk(int fst, int snd, int ind) : ind(ind) {
        x[0] = fst;
        x[1] = snd;
    }

    bool operator < (const DoubleChunk& other) {
        return (x[0] < other.x[0] || x[0] == other.x[0] && x[1] < other.x[1]);
    }
};

class SuffixArray {
public:
    int n;
    string s;

    // Call computeSA to populate arr and pos.
    // Call computeLCP to populate lcp.
    vector<int> arr; // arr[i] is index of suffix on position i of the suffix array. 
    // It is indexed from 0 to n, and contains a perumutation of numbers from 0 to n
    vector<int> pos; // pos[i] is the position of suffix i in the suffix array. 
    // It is indexed from 0 to n, and contains a permutation of numbers from 0 to n.
    vector<int> lcp; // lcp[i] is the lcp between suffixes arr[i] and arr[i+1].
    // It is indexed from 0 to n-1.

    void computeSA() {
        arr.resize(n + 1);
        pos.resize(n + 1);
        vector<DoubleChunk> chunks(n);
        
        for (int i = 0; i < n; ++i) {
            pos[i] = s[i];
        }

        for (int step = 1; step <= n; step <<= 1) {
            for (int i = 0; i < n; ++i) {
                chunks[i] = DoubleChunk(pos[i], i + step < n ? pos[i + step] : 0, i);
            }

            sort(chunks.begin(), chunks.end());

            int distinct = 0;
            for (int i = 0; i < chunks.size(); ++i) {
                if (i == 0 || chunks[i-1] < chunks[i])
                    ++distinct;
                pos[chunks[i].ind] = distinct;
            }
        }

        arr[0] = n; pos[n] = 0; // Empty suffix.
        for (int i = 0; i < n; ++i) {
            arr[pos[i]] = i;
        }
    }

    void computeLCP() {
        lcp.resize(n);
        lcp[0] = 0;

        int cr = 0;
        for (int i = 0; i < n; ++i) {
            if (pos[i] == n)
                continue;
            
            int nxt = arr[pos[i]+1];
            for (; i+cr < n && nxt+cr < n && s[i+cr] == s[nxt+cr]; ++cr);
            
            lcp[pos[i]] = cr;
            
            if (cr)
                --cr;
        }
    }


    SuffixArray(const string& s) : s(s) {
        n = s.length();
        computeSA();
    }
};

}

int main() {
    string s = "abbbacbaabb";
    strings::SuffixArray sa(s);
    sa.computeSA();
    sa.computeLCP();
    for (int i = 1; i <= s.length(); ++i) {
        cout << s.substr(sa.arr[i], s.length()) << "\n";
        if (i != s.length())
            cout << sa.lcp[i] << "\n";
    }

    cout << "\n";
}