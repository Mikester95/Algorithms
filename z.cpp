#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

namespace strings {

// Strings are indexed from 1, and start with a garbage character like #.
// Returns a vector saying for each i the length of the longest prefix of small
// That starts at position i in big.
vector<int> GetLens(const string& small, const string& big) {
    string huge = small + big;
    vector<int> ans(big.length()), z(huge.length());
    int N = huge.length()-1, l = 0, r = 0;
    for (int i = 2; i <= N; ++i) {
        if (r >= i) {
            z[i] = min(r-i+1, z[i-l+1]);
        }

        while (i+z[i] <= N && huge[i+z[i]] == huge[1+z[i]])
            ++z[i];

        if (i + z[i]-1 > r) {
            l = i;
            r = i+z[i]-1;
        }
    }

    for (int i = small.length()+1; i <= N; ++i) {
        ans[i-small.length()] = z[i];
    }

    return ans;
}

}

int main() {
    ifstream fin("strmatch.in");
    ofstream fout("strmatch.out");
    string s, S;
    fin >> s >> S;
    s = "#" + s;
    S = "#" + S;
    auto len = strings::GetLens(s, S);
    int ans = 0;
    for (int i = 1; i < S.length(); ++i) {
        if (len[i] == s.length()-1)
            ++ans;
    }
    fout << ans << "\n";
    ans = 0;
    for (int i = 1; i < S.length() && ans < 1000; ++i) {
        if (len[i] == s.length()-1) {
            ++ans;
            fout << i-1 << " ";
        }
    }
}