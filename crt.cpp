#include <iostream>
#include <vector>

using namespace std;

namespace crt {
 
template<typename T=long long>
void Euclidean(const T& a, const T& b, T& x, T& y, T& d) {
    if (b == 0) {
        d = a;
        x = 1;
        y = 0;
        return;
    }
    T x0, y0;
    Euclidean(b, a%b, x0, y0, d);
    x = y0;
    y = x0 - a/b * y0;   
}
 
template<typename T=long long>
T CarefulMult(const T& a, long long b, const T& mod) {
    if (b == 0)
        return 0;
 
    T x = CarefulMult(a, b>>1, mod);
 
    if (b&1) {
        return ((x + x) % mod + a) % mod;
    } else {
        return (x + x) % mod;
    }
}
 
template<typename T=long long, typename R=long long>
class CRT {
  public:
    // Returns pair (value, mod) of answer, or (-1, -1) if there is no answer.
    // 2 * (Final mod value) should be at most capcity(T). Algorithm uses careful
    // multiplication.
    pair<T, T> Solve(const vector<R>& a, const vector<R>& b) {
        T mod = a[0];
        T val = b[0];
        int n = a.size();
        for (int i = 1; i < n; ++i) {
            T x, y, gcd;
            Euclidean(mod, a[i], x, y, gcd);
            if ((b[i] - val) % gcd != 0) {
                return {-1, -1};
            }
            T new_mod = mod / gcd * a[i];
            // x <= a[i] from Euclidean, (and b[i] - val) <= mod.
            x = (b[i] - val) / gcd * x;
            val = (CarefulMult(x, mod, new_mod) + val) % new_mod;
            mod = new_mod;
            if (val < 0)
                val += mod;
        }
        return {val, mod};
    }
};
 
}