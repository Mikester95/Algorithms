#include <iostream>
#include <vector>

using namespace std;

// Static Matrix class.

const int sz = 16;
typedef long long int64;


template<typename T = int64, int N = sz, int M = sz>
struct Matrix {
    T a[N][M];

    Matrix(int x = 0) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                if (i == j) {
                    a[i][j] = x;
                } else {
                    a[i][j] = 0;
                }
            }
        }
    }

    Matrix(const vector<vector<T>>& b) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                a[i][j] = b[i][j];
            }
        }
    }

    template<int L>
    Matrix<T, N, L> operator * (Matrix<T, M, L>& other) {
        Matrix<T, N, L> res;

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < L; ++j) {
                for (int k = 0; k < M; ++k) {
                    res.a[i][j] += a[i][k] * other.a[k][j];
                }
            }
        }

        return res;
    }

    vector<T> operator * (const vector<T>& op) {
        vector<T> res;

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                res[i] += a[i][j] * op[j];
            }
        }

        return res;
    }

    Matrix<T, N, M> Exp(int64 pw) {
        Matrix<T, N, M> res(1);
        auto a = *this;

        for (int i = 0; (1LL<<i) <= pw; ++i) {
            if ((pw>>i)&1) {
                res = res * a;
            }
            a = a * a;
        }

        return res;
    }
};

int main() {
    Matrix<int, 3, 3> a(2), b(1);

    a = a.Exp(2);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << a.a[i][j] << "\n";
        }
    }
}