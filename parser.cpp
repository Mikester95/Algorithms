#include <cstring>
#include <cstdio>

using namespace std;

template<int cap = 1000000> // 1 Megabyte.
class Parser {
	char s[cap];
	int p, len;
	FILE* f;

	char get_char() {
		if (p == len) {
			fread(s, 1, cap-1, f);
			s[cap-1] = 0;
			len = strlen(s);
			p = 0;
		}
		char c = s[p];
		++p;
		return c;
	}

  public:
	Parser(FILE* f = stdin) : f(f) {
		p = 0;
		len = 0;
	}

	Parser& operator >> (int& x) {
		char c;
		while (isspace(c = get_char()));

		x = c - '0';
		while(!isspace(c = get_char())) {
			x = x * 10 + c - '0';
		}

		return *this;
	}
};