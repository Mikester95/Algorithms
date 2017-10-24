#include <map>
#include <unordered_map>

using namespace std;

class DSU {
    map<int, int> t, d;
    int total, merges;

    int parent_compression(int a) {
        if (t[a]) {
            return t[a] = parent_compression(t[a]);
        }
        return a;
    }

  public:
    DSU() : total(0), merges(0) {}

    void unite(int a, int b) {
        a = parent_compression(a);
        b = parent_compression(b);

        if (a == b)
            return;

        ++merges;
        if (d[a] < d[b]) {
            t[a] = b;
        } else {
            t[b] = a;
            if (d[a] == d[b]) {
                ++d[a];
            }
        }
    }

    int get_parent(int a) {
        if (t[a]) {
            return get_parent(t[a]);
        }
        return a;
    }

    int get_merges() {
        return merges;
    }
};