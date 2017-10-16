#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef long long int64;

class Huge {
    static const int kLogBase = 9;
    static int pw10(int x) {
        int r = 1;
        for (int i = 0; i < x; ++i) {
            r *= 10;
        }
        return r;
    }
    static const int kBase;
    vector<int64> v;

  public:
    inline int size() const {
        return (int)v.size();
    }

    void Append(int64 x) {
        while (x) {
            v.push_back(x % kBase);
            x /= kBase;
        }
    }

    void Trim() {
        while (size() > 0 && v.back() == 0) {
            v.pop_back();
        }
    }

    Huge() {}

    Huge(int64 x) {
        v.clear();
        Append(x);
    }

    int64& operator [] (int x) {
        return v[x];
    }

    int64 operator [] (int x) const {
        return v[x];
    }

    bool operator < (const Huge& other) const {
        if (other.size() != size()) {
            return size() < other.size();
        }
        for (int i = size()-1; i >= 0; --i) {
            if (v[i] != other[i]) {
                return v[i] < other[i];
            }
        }
        return false;
    }

    bool operator > (const Huge& other) const {
        if (other.size() != size()) {
            return size() > other.size();
        }
        for (int i = size()-1; i >= 0; --i) {
            if (v[i] != other[i]) {
                return v[i] > other[i];
            }
        }
        return false;
    }

    bool operator <= (const Huge& other) const {
        return !(*this > other);
    }

    bool operator >= (const Huge& other) const {
        return !(*this < other);
    }

    bool operator == (const Huge& other) {
        return v == other.v;
    }

    bool operator != (const Huge& other) {
        return v != other.v;
    }

    void operator <<= (int x) {
        v.insert(v.begin(), x, 0);
    }

    void operator >>= (int x) {
        v.erase(v.begin(), v.begin() + x);
    }   

    void operator += (const Huge& other) {
        int sz = max(other.size(), size());
        v.resize(sz);

        int transport = 0;
        for (int i = 0; i < other.size() || (i < size() && transport); ++i) {
            v[i] += (i < other.size() ? other[i] : 0) + transport;
            if (v[i] >= kBase) {
                transport = 1;
                v[i] -= kBase;
            } else {
                transport = 0;
            }
        }

        Append(transport);
    }

    Huge operator + (const Huge& other) const {
        Huge result = *this;
        result += other;
        return result;
    }

    void operator -= (const Huge& other) {
        int transport = 0;
        for (int i = 0; i < other.size() || (i < size() && transport); ++i) {
            v[i] -= (i < other.size() ? other[i] : 0) + transport;
            if (v[i] < 0) {
                v[i] += kBase;
                transport = 1;
            } else {
                transport = 0;
            }
        }
        if (transport) {
            cerr << "You tried to make a negative number! Not supported!";
        }
        Trim();
    }

    Huge operator - (const Huge& other) const {
        Huge result = *this;
        result -= other;
        return result;
    }

    Huge operator * (const Huge& other) const {
        Huge result;
        result.v.resize(size() + other.size());
        for (int i = 0; i < size(); ++i) {
            for (int j = 0; j < other.size(); ++j) {
                result[i+j] += v[i] * other[j];
                for (int k = i+j; result[k] >= kBase; ++k) {
                    result[k+1] += result[k] / kBase;
                    result[k] %= kBase;
                }
            }
        }

        result.Trim();

        return result;
    }

    void operator *= (const Huge& other) {
        *this = *this * other;
    }

    int64 operator /= (int64 div) {
        int64 rem = 0;
        for (int i = size() - 1; i >= 0; --i) {
            rem = rem * kBase + v[i];
            v[i] = rem / div;
            rem %= div;
        }
        Trim();
        return rem;
    }

    pair<Huge, int64> operator / (int64 div) const {
        Huge result = *this;
        int64 rem = result /= div;
        return {result, rem};
    }

    Huge operator /= (const Huge& div) {
        Huge rem;
        for (int i = size() - 1; i >= 0; --i) {
            rem <<= 1;
            rem[0] = v[i];
            int64 lo = 0, hi = kBase;
            while (hi - lo > 1) {
                int64 mid = (hi + lo) >> 1;
                if (div * mid <= rem) {
                    lo = mid;
                } else {
                    hi = mid;
                }
            }
            v[i] = lo;
            rem -= div * lo;
        }
        Trim();
        return rem;
    }

    pair<Huge, Huge> operator / (const Huge& div) const {
        Huge result = *this;
        Huge rem = result /= div;
        return {result, rem};
    }

    friend ostream& operator << (ostream& out, const Huge& huge);
};

const int Huge::kBase = Huge::pw10(Huge::kLogBase);

ostream& operator << (ostream& out, const Huge& huge) {
    if (huge.size() == 0) {
        out << '0';
        return out;
    }
    string s;
    for (int i = 0; i < huge.size(); ++i) {
        int64 x = huge[i];
        for (int j = 0; j < Huge::kLogBase; ++j) {
            s += (char)(x % 10 + '0');
            x /= 10;
        }
    }
    // out << s << "\n";
    while (s.size() > 0 && s.back() == '0')
        s.pop_back();
    reverse(s.begin(), s.end());
    out << s;
    return out;
}

int main() {

    cout << (h == k);
}
