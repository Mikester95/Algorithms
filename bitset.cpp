#include <iostream>
#include <vector>

using namespace std;

typedef long long int64;

class Bitset {
    int n;
    vector<int64> v;
    static vector<short> popcount; // Be careful. 4 Megabytes will be allocated here.

    static int64 LSB(int64 i) {
        return i&-i;
    }

    static void PrecomputePopcount() {
        popcount.resize(1<<21);
        for (int i = 1; i < (1<<21); ++i) {
            popcount[i] = popcount[i - LSB(i)] + 1;
        }
    }

  public:
    int size() const {
        return n;
    }

    void resize(int n) {
        this->n = n;
        v.resize((n >> 6) + 1);
    }

    Bitset() {
        n = 0;
    }
    Bitset(int n) : n(n) {
        v = vector<int64>((n >> 6) + 1);
    }

    int get(int p) const {
        return (v[p >> 6] >> (p & 63))&1;
    }

    void set(int p, int bit) {
        if (bit != get(p)) {
            v[p >> 6] ^= (1LL << (p & 63));
        }
    }

    int count() const {
        if (popcount.empty())
            PrecomputePopcount();
        int ans = 0;
        int full_mask = ((1<<21)-1);
        for (int i = 0; i < v.size(); ++i) {
            ans += popcount[v[i] & full_mask] + popcount[(v[i] >> 21) & full_mask] +
                popcount[(v[i] >> 42) & full_mask] + ((v[i] >> 63)&1);
        }
        return ans;
    }

    void operator &= (const Bitset& other) {
        if (other.size() < size())
            resize(other.size());
        for (int i = 0; i < v.size(); ++i) {
            v[i] &= other.v[i];
        }
    }

    Bitset operator & (const Bitset& other) {
        Bitset result = *this;
        result &= other;
        return result;
    }

    void operator |= (const Bitset& other) {
        if (other.size() > size())
            resize(other.size());
        for (int i = 0; i < other.size(); ++i) {
            v[i] |= other.v[i];
        }
    }

    Bitset operator | (const Bitset& other) {
        Bitset result = *this;
        result |= other;
        return result;
    }

    friend ostream& operator << (ostream& out, const Bitset& b);
    friend istream& operator >> (istream& in, Bitset& b);
};

vector<short> Bitset::popcount = {};

ostream& operator << (ostream& out, const Bitset& b) {
    string s;
    for (int i = 0; i < b.size(); ++i) {
        s += (char)(b.get(i) + '0');
    }
    out << s;
    return out;
}

istream& operator >> (istream& in, Bitset& b) {
    string s;
    in >> s;
    b = Bitset(s.size());
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == '1')
            b.set(i, 1);
        else b.set(i, 0);
    }
    return in;
}

int main() {
    Bitset b(30);
    b.set(1, 1);
    cout << b << "\n";
    b.set(1, 0);
    b.set(2, 1);
    b.set(2, 1);
    cout << b << "\n";
}