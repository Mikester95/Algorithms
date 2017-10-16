#include <iostream>
#include <vector>

using namespace std;

typedef long long int64;

struct Mod {
    static int mod;
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
        if (this->x >= mod)
            this->x %= mod;
        if (this->x < 0)
            this->x += mod;
    }
    
    bool operator == (const Mod& other) {
        return x == other.x;
    }
    
    bool operator != (const Mod& other) {
        return x != other.x;
    }
    
    void operator += (const Mod& other) {
        x += other.x;
        if (x >= mod)
            x -= mod;
    }
    
    Mod operator + (const Mod& other) const {
        Mod result = *this;
        result += other;
        return result;
    }
    
    void operator -= (const Mod& other) {
        x -= other.x;
        if (x < 0)
            x += mod;
    }
    
    Mod operator - (const Mod& other) const {
        Mod result = *this;
        result -= other;
        return result;
    }
    
    void operator *= (const Mod& other) {
        x *= other.x;
        x %= mod;
    }
    
    Mod operator * (const Mod& other) const {
        Mod result = *this;
        result *= other;
        return result;
    }
    
    Mod Inv() const {
        int64 a, b;
        Euclidean(x, mod, a, b);
        return a;
    }
    
    Mod operator - () const {
        Mod result = *this;
        result.x = mod - result.x;
        return result;
    }
    
    void operator /= (const Mod& other) {
        *this *= other.Inv();
    }
    
    Mod operator / (const Mod& other) const {
        Mod result = *this;
        result /= other;
        return result;
    }
};

int Mod::mod = 1e9+7;

ostream& operator << (ostream& out, const Mod& m) {
    out << m.x;
    return out;
}

istream& operator >>(istream& in, const Mod& m) {
    in >> m.x;
    return in;
}

Mod operator + (int64 x, const Mod& other) {
    return other + x;
}

Mod operator - (int64 x, const Mod& other) {
    return x + (-other);
}

Mod operator * (int64 x, const Mod& other) {
    return other * x;
}

Mod operator / (int64 x, const Mod& other) {
    return other.Inv() * x;
}

class Combinatorics {
    vector<Mod> fac, inv_fac;    
    vector<vector<Mod>> comb;

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
    
    Mod Fac(int n) {
        return fac[n];
    }
    
    Mod InvFac(int n) {
        return inv_fac[n];
    }
    
    Mod Comb(int n, int k) {
        if (n < 0 || k > n)
            return 0;
        if (comb.empty()) {
            return fac[n] * inv_fac[k] * inv_fac[n - k];
        }
        return 0;
    }
};

int main() {
    
}