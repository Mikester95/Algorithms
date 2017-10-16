#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

namespace fft {

// General Convolution classes.

// DFT uses FFT over the field of complex numbers.
// NTT uses FFT over the field of numbers modulo 2013265921.

// How to use: 
// 1. Instantiate an object of class DFT or NTT.
// convolution::DFT dft;
// 2. Call the convolute method on two std::vectors a and b.
// auto c = dft.convolute(a, b);

// In the case of DFT, the types can be double, int or int64.
// In the case of NTT, the types can be int, int64 or unsigned int.

// DFT is faster and works on larger numbers, but may have precision issues.
// NTT should only be used when the inputs and outputs fit in the range
// [-10^9, 10^9] for int or int64, or [0, 2*10^9] for unsigned in.
// The array sizes for NTT should be at most 2^26.

typedef long long int64;

class Complex {
public:
    static constexpr double pi = acos(-1);
    double a, b;

    Complex(double a=0, double b=0) : a(a), b(b) {}

    Complex operator + (const Complex& other) const {
        return Complex(a + other.a, b + other.b);
    }

    Complex operator - (const Complex& other) const {
        return Complex(a - other.a, b - other.b);
    }

    Complex operator * (const Complex& other) const {
        return Complex(a * other.a - b * other.b, a * other.b + b * other.a);
    }

    Complex inverse() const {
        double len = a*a + b*b;
        return Complex(a / len, -b / len);
    }

    static Complex root(int lgsz, int kth=1) {
        return Complex(cos(2*pi*kth / (1<<lgsz)), sin(2*pi*kth / (1<<lgsz)));
    }
};

ostream& operator << (ostream& out, const Complex& c) {
    out << c.a << " + " << c.b << "i";
    return out;
}

void assign(double& d, const Complex& c) {
    d = c.a;
}

int64 myRound(const double& d) {
    return d + (d < 0 ? -0.5 : 0.5);
}

void assign(Complex& myC, const Complex& c) {
	myC = c;
}

void assign(int64& i, const Complex& c) {
    i = myRound(c.a);
}

void assign(int& i, const Complex& c) {
    i = myRound(c.a);
}

class Modular {
  public:
    static const int64 mod = 2013265921;
    static const int64 r = 137;
    static int64 lgpow(int64 x, int64 pw) {
        if (pw == 0)
            return 1;
        auto x2 = lgpow(x, pw>>1);
        if (pw&1)
            return x2*x2%mod*x%mod;
        return x2*x2%mod;
    }
    int64 x;

    Modular(int64 x=0) {
        if (x >= mod || x <= -mod)
            x %= mod;
        this->x = x;
    }

    Modular operator + (const Modular& other) const {
        int64 s = x + other.x;
        if (s >= mod)
            s -= mod;
        return Modular(s);
    }

    Modular operator - (const Modular& other) const {
        int64 d = x - other.x;
        if (d <= -mod)
            d += mod;
        return Modular(d);
    }

    Modular operator * (const Modular& other) const {
        return Modular(x * other.x % mod);
    }

    Modular inverse() {
        return Modular(lgpow(x, mod-2));
    }

    static Modular root(int lgsz, int kth=1) {
        return Modular(lgpow(lgpow(r, 1<<(27-lgsz)), kth));
    }
};

ostream& operator << (ostream& out, const Modular& m) {
    out << m.x;
    return out;
}

void assign(int64& i, const Modular& m) {
    auto t = m.x;
    if (t < 0)
        t += Modular::mod;
    i = t;
    if (t >= (Modular::mod>>1))
        i = -(Modular::mod - t);
}

void assign(int& i, const Modular& m) {
	auto t = m.x;
    if (t < 0)
        t += Modular::mod;
    i = t;
    if (t >= (Modular::mod>>1))
        i = -(Modular::mod - t);
}

void assign(unsigned int& i, const Modular& m) {
    auto t = m.x;
    if (t < 0)
        t += Modular::mod;
    i = t;
}

template<typename C>
class Convolution {
    int sz, lgsz;
    vector<C> roots, invRoots, lhs, rhs, res;
    vector<int> rev;

    static int nextPower2(int sz) {
        int i = 0;
        while ((1<<i) < sz)
            ++i;
        return i;
    }

    void precalc() {
        if (2*sz != roots.size()) {
            roots.resize(2*sz);
            invRoots.resize(2*sz);
            for (int j = (1<<lgsz); j < (1<<(lgsz+1)); ++j) {
                roots[j] = C::root(lgsz, j - (1<<lgsz));
                invRoots[j] = roots[j].inverse();
            }
            for (int j = (1<<lgsz) - 1; j >= 1; --j) {
                roots[j] = roots[j<<1];
                invRoots[j] = invRoots[j<<1];
            }        
        }

        if (sz != rev.size()) {
            rev.resize(sz);
            rev[0] = 0;
            for (int i = 1; i < sz; ++i) {
                rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (lgsz - 1));
            }
        }
    }

    void prepInverse() {
        res.resize(sz);
        for (int i = 0; i < sz; ++i) {
            res[i] = lhs[i] * rhs[i];
        }
    }

    void fft(vector<C>& c, bool invert) {
        for (int i = 0; i < sz; ++i) {
            if (i < rev[i])
                swap(c[i], c[rev[i]]);
        }

        auto& r = (invert ? invRoots : roots);

        for (int halfStep = 1; halfStep < sz; halfStep <<= 1) {
            for (int start = 0; start < sz; start += (halfStep<<1)) {
                for (int i = 0; i < halfStep; ++i) {
                    C w = r[i + (halfStep<<1)] * c[start + i + halfStep];
                    c[start + i + halfStep] = c[start + i] - w;
                    c[start + i] = c[start + i] + w;
                }
            }
        }

        if (invert) {
            C mult = C(sz).inverse();
            for (int i = 0; i < sz; ++i) {
                c[i] = c[i] * mult;
            }
        }
    }

    template<typename T>
    void toField(vector<C>& c, const vector<T>& a) {
        c.resize(sz);
        for (int i = 0; i < a.size(); ++i)
            c[i] = a[i];
        for (int i = a.size(); i < sz; ++i)
            c[i] = 0;
    }

    template<typename T>
    void fromField(vector<T>& a, const vector<C>& c) {
        a.resize(sz);
        for (int i = 0; i < sz; ++i) {
            assign(a[i], c[i]);
        }
    }

  public:
    template<typename T>
    vector<T> convolute(const vector<T>& lhs, const vector<T>& rhs) {
        lgsz = 1 + nextPower2(max(lhs.size(), rhs.size()));
        sz = (1<<lgsz);
        precalc();
        toField(this->lhs, lhs);
        toField(this->rhs, rhs);
        fft(this->lhs, false);
        fft(this->rhs, false);
        prepInverse();
        fft(this->res, true);
        vector<T> res;
        fromField(res, this->res);
        return res;
    }
};

typedef Convolution<Complex> DFT;
typedef Convolution<Modular> NTT;
};

int main() {
	vector<int> a{30000, 1, 0}, b{40000, 1, 0};
    
    fft::DFT dft;
	auto c = dft.convolute(a, b);
	for (int i = 0; i < c.size(); ++i) {
		cout << c[i] << "\n";
	}
}