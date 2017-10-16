#include <iostream>
#include <vector>

using namespace std;

class RMQ {
    vector<int> log, arr;
    vector<vector<int>> rmq;
  public:
    int n;
    RMQ() {}
    RMQ(const vector<int>& arr) : arr(arr) {
        int n = arr.size() - 1;
        log.resize(n+1);
        log[1] = 0;
        for (int i = 2; i <= n; ++i) {
            log[i] = log[i>>1] + 1;
        }

        rmq = vector<vector<int>>(log[n]+1, vector<int>(n+1));

        for (int i = 1; i <= n; ++i)
            rmq[0][i] = i;
        for (int lg = 1; lg <= log[n]; ++lg) {
            for (int i = 1; i + (1<<lg) - 1 <= n; ++i) {
                if (arr[rmq[lg-1][i]] < arr[rmq[lg-1][i+(1<<(lg-1))]])
                    rmq[lg][i] = rmq[lg-1][i];
                else rmq[lg][i] = rmq[lg-1][i+(1<<(lg-1))];
            }
        }
    }

    int query(int a, int b) {
        if (a > b)
            swap(a, b);
        int len = b - a + 1;
        int sol = rmq[log[len]][a];
        if (arr[sol] > arr[rmq[log[len]][b - (1<<(log[len])) +1]])
            sol = rmq[log[len]][b - (1<<(log[len])) + 1];
        return sol;
    }
};

int main() {
}