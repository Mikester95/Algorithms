#include <vector>
#include <iostream>

using namespace std;

// Fenwick Tree(or Binary Indexed Tree) structure that supports queries on prefixes, and point-updates
// for with an associative, commutative operation. 
// Queries on intervals can be performed when the operation is also invertible.

// Type T must have operator += defined on it, 
// as well as operators +, <= and < if LowerBound/UpperBound are used.

template<typename T>
class FenTree {
    int n;
    const int lg;
    vector<T> aib;

    static inline int LSB(int i) {
        return i&-i;
    }

    static inline int GetLg(int n) {
        int lg = 0;
        while ((1<<lg) <= n)
            ++lg;
        --lg;
        return lg;
    }

  public:
    FenTree(int n) : n(n), lg(GetLg(n)) {
        aib.resize(n+1);
    }

    // Apply (+ val) to element on position i.
    void Update(int i, T val) {
        for (; i <= n; i += LSB(i)) {
            aib[i] += val;
        }
    }

    // Query the prefix up to position i.
    T Query(int i) {
        T res = 0;
        for (; i > 0; i -= LSB(i)) {
            res += aib[i];
        }
        return res;
    }

    // Searches for the first position where the prefix sum is at least greater than x
    // (first greater than x).
    int UpperBound(T x) {
        int current = 0;
        T taken = 0;
        for(int bit = lg; bit >= 0; --bit) {
            if (current + (1 << bit) <= n && taken + aib[current + (1 << bit)] <= x) {
                taken += aib[current + (1 << bit)];
                current += (1 << bit);
            }
        }

        return current + 1;
    }

    // Searches for the first position where the prefix sum is at least x
    // (first equal to x).
    int LowerBound(T x) {
        int current = 0;
        T taken = 0;
        for(int bit = lg; bit >= 0; --bit) {
            if (current + (1 << bit) <= n && taken + aib[current + (1 << bit)] < x) {
                taken += aib[current + (1 << bit)];
                current += (1 << bit);
            }
        }

        return current + 1; 
    }
};

int main() {
    FenTree<int> fen(10);
    fen.Update(2, 3);
    fen.Update(3, 4);
    fen.Update(6, 2);

    cout << fen.LowerBound(0) << "\n" << fen.LowerBound(3) << "\n" << fen.LowerBound(9) << "\n";
}