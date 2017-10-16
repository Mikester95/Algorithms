#include <vector>

using namespace std;

template<typename T>
struct FenTree {
	int n;
	vector<T> aib;
	FenTree(int n) : n(n) {
		aib.resize(n+1);
	}

	static inline int LSB(int i) {
		return i&-i;
	}

	void update(int i, T val) {
		for (; i <= n; i += LSB(i)) {
			aib[i] += val;
		}
	}

	int query(int i) {
		T res = 0;
		for (; i > 0; i -= LSB(i)) {
			res += aib[i];
		}
		return res;
	}
};