#include <iostream>
#include <vector>

using namespace std;

typedef long long int64;

const int kMod = 1e9+7;

template<int M = kMod>
struct Mod {
    int64 x;
    
    static void Euclidean(int64 a, int64 b, int64& x, int64& y) {
        if (b == 0) {
            x = 1;
            y = 0;
            return;
        }
        int64 x0, y0;
        Euclidean(b, a%b, x0, y0);
        x = y0;
        y = x0 - a/b * y0;   
    }
    
    Mod() : x(0) {}
    
    Mod(int64 x) : x(x) {
        if (this->x >= M)
            this->x %= M;
        if (this->x < 0)
            this->x += M;
    }
    
    bool operator == (const Mod<M>& other) {
        return x == other.x;
    }
    
    bool operator != (const Mod<M>& other) {
        return x != other.x;
    }
    
    void operator += (const Mod<M>& other) {
        x += other.x;
        if (x >= M)
            x -= M;
    }
    
    Mod<M> operator + (const Mod<M>& other) const {
        Mod<M> result = *this;
        result += other;
        return result;
    }
    
    void operator -= (const Mod<M>& other) {
        x -= other.x;
        if (x < 0)
            x += M;
    }
    
    Mod<M> operator - (const Mod<M>& other) const {
        Mod<M> result = *this;
        result -= other;
        return result;
    }
    
    void operator *= (const Mod<M>& other) {
        x *= other.x;
        x %= M;
    }
    
    Mod<M> operator * (const Mod<M>& other) const {
        Mod<M> result = *this;
        result *= other;
        return result;
    }
    
    Mod<M> Inv() const {
        int64 a, b;
        Euclidean(x, M, a, b);
        return a;
    }
    
    Mod<M> operator - () const {
        Mod<M> result = *this;
        result.x = M - result.x;
        return result;
    }
    
    void operator /= (const Mod<M>& other) {
        *this *= other.Inv();
    }
    
    Mod<M> operator / (const Mod<M>& other) const {
        Mod<M> result = *this;
        result /= other;
        return result;
    }
};

template<int M = kMod>
ostream& operator << (ostream& out, const Mod<M>& m) {
    out << m.x;
    return out;
}

template<int M = kMod>
istream& operator >>(istream& in, const Mod<M>& m) {
    in >> m.x;
    return in;
}


template<int M = kMod>
Mod<M> operator + (int64 x, const Mod<M>& other) {
    return other + x;
}

template<int M = kMod>
Mod<M> operator - (int64 x, const Mod<M>& other) {
    return x + (-other);
}

template<int M = kMod>
Mod<M> operator * (int64 x, const Mod<M>& other) {
    return other * x;
}

template<int M = kMod>
Mod<M> operator / (int64 x, const Mod<M>& other) {
    return other.Inv() * x;
}

template<int M = 1e9+7>
class Combinatorics {
    vector<Mod<M>> fac, inv_fac;    
    vector<vector<Mod<M>>> comb;

  public:
    void PrecomputeFactorials(int n) {
        fac.resize(n+1);
        inv_fac.resize(n+1);
        
        fac[0] = 1;
        for (int i = 1; i <= n; ++i) {
            fac[i] = fac[i-1] * i;
        }
        
        inv_fac[n] = fac[n].Inv();
        for (int i = n-1; i >= 0; --i) {
            inv_fac[i] = inv_fac[i+1] * (i+1);
        }
    }
    
    Mod<M> Fac(int n) {
        return fac[n];
    }
    
    Mod<M> InvFac(int n) {
        return inv_fac[n];
    }
    
    Mod<M> Comb(int n, int k) {
        if (n < 0 || k > n)
            return 0;
        if (comb.empty()) {
            return fac[n] * inv_fac[k] * inv_fac[n - k];
        }
        return 0;
    }
};

int main() {
    Mod<> m(3);

    cout << m;

    m = 3 + m;

    cout << m;
}