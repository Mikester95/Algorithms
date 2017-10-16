#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

class BigNum {
    int v[3500];
    int sz;

  public:
    static const int base = (1<<30);

    int size() const {
        return sz;
    }

    BigNum () : sz(0) {}

    BigNum(long long c) {
        sz = 0;
        complete(c);
    }

    void complete(long long c) {
        while (c) {
            v[sz++] = (c%base);
            c /= base;
        }
    }

    void resize(int new_sz) {
        new_sz = max(new_sz, 0);
        if (new_sz < sz) {
            for (int i = sz-1; i >= new_sz; --i)
                v[i] = 0;
        } else {
            for (int i = sz; i < new_sz; ++i)
                v[i] = 0;
        }
        sz = new_sz;
    }

    int& operator [] (int x) {
        return v[x];
    }

    int operator [] (int x) const {
        return v[x];
    }

    void operator = (const BigNum& other) {
        for (int i = 0; i < other.sz; ++i) {
            v[i] = other.v[i];
        }
        sz = other.sz;
    }

    void operator *= (long long c) {
        int t = 0;
        for (int i = 0; i < sz; ++i) {
            long long x = v[i] * c + t;
            t = (x>>30);
            v[i] = (x&(base-1));
        }
        complete(t);
    }

    int operator /= (int div) {
        long long rem = 0;
        for (int i = sz-1; i >= 0; --i) {
            rem = (rem<<30) + v[i];
            v[i] = rem / div;
            rem %= div;
        }
        while (sz > 0 && v[sz-1] == 0)
            --sz;
        return rem;
    }

    void operator += (const BigNum& other) {
        while (sz < other.sz)
            v[sz++] = 0;
        int t = 0;
        int i;
        for (i = 0; i < other.sz || t; ++i) {
            v[i] += t + (i < other.sz ? other.v[i] : 0);
            t = (v[i]>>30);
            v[i] &= (base-1);
        }
    }

    void operator <<= (int c) {
        for (int i = sz-1; i >= 0; --i) {
            v[i+c] = v[i];
        }
        for (int i = 0; i < c; ++i)
            v[i] = 0;
        sz += c;
    }

    bool less_than_or_equal_to_with_left_shift(const BigNum& other, int shift) {
        int st = max(sz, other.sz + shift);
        for (int i = st-1; i >= 0; --i) {
            int c1 = (i < sz ? v[i] : 0);
            int c2 = (i - shift >= 0 && i - shift < other.sz ? other.v[i-shift] : 0);
            if (c1 < c2) {
                return true;
            } else if (c1 > c2) {
                return false;
            }
        }
        return true;
    }
};

int n, k;

void split(const BigNum& a, BigNum& left, BigNum& right, int leftsize) {
    left.resize(leftsize);
    for (int i = 0; i < leftsize; ++i) {
        left[i] = a[i];
    }
    right.resize(a.size() - leftsize);
    for (int i = leftsize; i < a.size(); ++i) {
        right[i-leftsize] = a[i];
    }
}

string to_str(int x) {
    string s;
    for (int i = 0; i < 9; ++i) {
        s += (char)(x%10 + '0');
        x /= 10;
    }
    return s;
}

int leading_zeroes(const string& repr) {
    int i = 0;
    while (i < repr.size() && repr[i] == '0')
        ++i;
    if (i == repr.size())
        return i-1;
    else return i;
}

string base_10_representation(BigNum x, int digits_after_decimal, int wanted_digits_after_decimal) {
    BigNum a, b;
    split(x, a, b, digits_after_decimal);

    int new_base = 1e9;

    string before_decimal;
    while (b.size() > 0) {
        int rem = (b /= new_base);
        string s = to_str(rem);
        before_decimal += s;
    }
    reverse(before_decimal.begin(), before_decimal.end());
    int num_zeroes = leading_zeroes(before_decimal);
    before_decimal.erase(0, num_zeroes);

    string after_decimal;
    for (int i = 0; i < digits_after_decimal; ++i) {
        a.resize(a.size() + 1);
        a *= new_base;
        int x = a[a.size() - 1];
        a.resize(a.size()- 1);
        string s = to_str(x);
        reverse(s.begin(), s.end());
        after_decimal += s;
    }
    while (after_decimal.size() > wanted_digits_after_decimal)
        after_decimal.pop_back();
    return before_decimal + "." + after_decimal;
}

string Sqrt(BigNum N, int k) {
    if (N.size() % 2 == 1) {
        N.resize(N.size() + 1);
    }

    int logbase = log10(N.base);

    int digits_before_decimal = N.size()/2;
    int digits_after_decimal = k / logbase + 1;
    int total_digits = digits_before_decimal + digits_after_decimal;

    BigNum ans(0), square(0);
    // Binary search each digit of the result.
    // Use the fact that (x * 10 + d)^2 = x^2 * 100 + 2 * d * x + d^2.
    for (int i = 0; i < total_digits; ++i) {
        long long lo = 0, hi = N.base;
        ans <<= 1;
        square <<= 2;
        while (hi - lo > 1) {
            long long mid = (lo + hi)>>1;
            BigNum current_square = square;
            BigNum add = ans;
            add *= 2*mid;
            current_square += add;
            current_square += mid * mid;
            if (current_square.less_than_or_equal_to_with_left_shift(N, 2*i)) {
                lo = mid;
            } else {
                hi = mid;
            }
        }
        BigNum add = ans;
        add *= 2*lo;
        square += add;
        square += lo * lo;
        ans[0] = lo;
    }

    return base_10_representation(ans, digits_after_decimal, k);
}

int main()
{
    freopen("sqrt.in", "r", stdin);
    freopen("sqrt.out", "w", stdout);
    ios_base::sync_with_stdio(false);
    cin >> n >> k;
    cout << Sqrt(n, k);
}
