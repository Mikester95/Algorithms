#include <iostream>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <cassert>
#include <vector>

using namespace std;

typedef double T;

const T EPS = 1e-12;

bool equal(const T& a, const T& b) {
    return (fabs(a - b) < EPS);
}

const T pi = acos(-1);

struct Point {
    T x, y;

    Point() {}

    Point(const T& x, const T& y) : x(x), y(y) {}

    Point operator + (const Point& other) const {
        return {x + other.x, y + other.y};
    }

    Point operator - (const Point& other) const {
        return {x - other.x, y - other.y};
    }

    Point operator * (const T& c) const {
        return {x * c, y * c};
    }

    Point operator / (const T& c) const {
        return {x / c, y / c};
    }

    Point rotateCCW90() const {
        return {-y , x};
    }

    Point rotateCW90() const {
        return {y, -x};
    }

    Point rotate(T ang, const Point& around = Point(0, 0)) const {
        Point rot = *this - around;
        T cs = cos(ang);
        T sn = sin(ang);
        rot = Point(rot.x * cs - rot.y * sn, rot.x * sn + rot.y * cs);
        rot = rot + around;
        return rot;
    }

    bool operator == (const Point& other) const {
        return equal(x, other.x) && equal(y, other.y);
    }

    bool operator != (const Point& other) const {
        return !equal(x, other.x) || !equal(y, other.y);
    }

    bool operator < (const Point& other) const {
        if (!equal(x, other.x)) {
            return x < other.x;
        }
        return y < other.y;
    }

    T len() const {
        return sqrt(x*x + y*y);
    }

    T sqrlen() const {
        return x*x + y*y;
    }

    void normalize() {
        T l = len();
        x /= l;
        y /= l;
    }
};

istream& operator >> (istream& in, Point& p) {
    in >> p.x >> p.y;
    return in;
}

ostream& operator << (ostream& out, const Point& p) {
    out << "(" << p.x << ", " << p.y << ")";
    return out;
}

T dot(const Point& a, const Point& b) {
    return a.x * b.x + a.y * b.y;
}

T cross(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

T det(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

T sqrdist(const Point& a, const Point& b) {
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

T dist(const Point& a, const Point& b) {
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

bool colinear(const Point& a, const Point& b, const Point& c) {
    return fabs(det(a, b, c)) < EPS;
}

// Clockwise turn through b.
bool CW(const Point& a, const Point& b, const Point& c) {
    return det(a, b, c) < 0;
}

// Counterclockwise turn through b.
bool CCW(const Point& a, const Point& b, const Point& c) {
    return det(a, b, c) > 0;
}

// Right angle in b.
bool right(const Point& a, const Point& b, const Point& c) {
    return fabs(dot(a - b, c - b)) < EPS;
}

// Acute angle in b.
bool acute(const Point& a, const Point& b, const Point& c) {
    return dot(a - b, c - b) > 0;
}

// Obtuse angle in b.
bool obtuse(const Point& a, const Point& b, const Point& c) {
    return dot(a - b, c - b) < 0;
}

// Returns points on the convex hull in counterclocwise order.
// keep_colinear: boolean, if true keeps colinear points.
vector<Point> ConvexHull(vector<Point> v, bool keep_colinear=false) {
    sort(v.begin(), v.end());
    vector<Point> ch;

    vector<int> st(v.size());
    int k = 0;

    auto place = [&](int i) {
        while(k > 1 && (CW(v[st[k-2]], v[st[k-1]], v[i]) || 
            (!keep_colinear && colinear(v[st[k-2]], v[st[k-1]], v[i]))))
            --k;
        st[k++] = i;
    };

    for (int i = 0; i < v.size(); ++i) {
        place(i);
    }

    for (int i = 0; i < k; ++i) {
        ch.push_back(v[st[i]]);
    }

    k = 0;
    for (int i = v.size()-1; i >= 0; --i) {
        place(i);
    }

    for (int i = 1; i < k-1; ++i) {
        ch.push_back(v[st[i]]);
    }

    return ch;
}

// Project point p onto line formed by a and b.
Point ProjectPointLine(const Point& p, const Point& a, const Point& b) {
    Point ab = b - a;
    return a + ab * dot(p - a, ab) / ab.sqrlen();
}

// Project point p onto segment formed by a and b.
Point ProjectPointSegment(const Point& p, const Point& a, const Point& b) {
    if (a == b)
        return a;
    Point ab = b - a;
    T coef = dot(p - a, ab) / ab.sqrlen();
    if (coef < 0) return a;
    if (coef > 1) return b;
    return a + ab * coef;
}

T DistancePointLine(const Point& p, const Point& a, const Point& b) {
    return dist(p, ProjectPointLine(p, a, b));
}

T DistancePointSegment(const Point& p, const Point& a, const Point& b) {
    return dist(p, ProjectPointSegment(p, a, b));
}

// Checks if lines ab and cd are parallel
bool LinesParallel(const Point& a, const Point& b, const Point& c, const Point& d) {
    return fabs(cross(b - a, c - d)) < EPS;
}

// Checks if lines ab and cd are colinear.
bool LinesColinear(const Point& a, const Point& b, const Point& c, const Point& d) {
    return colinear(a, b, c) && colinear(a, b, d);
}

// Returns if c and d are strictly on the same side of line passing through a and b.
bool OnSameSide(const Point& a, const Point& b, const Point& c, const Point& d) {
    return CW(a, b, c) && CW(a, b, d) || CCW(a, b, c) && CCW(a, b, d);
}

// Determine if segments ab and ac intersect.
bool SegmentsIntersect(const Point& a, const Point& b, const Point& c, const Point& d) {
    if (LinesColinear(a, b, c, d)) {
        if (a == c || a == d || b == c || b == d)
            return true;
        if (obtuse(a, b, c) && obtuse(a, b, d) ||
            obtuse(b, a, c) && obtuse(b, a, d))
            return false;
        return true;
    }
    if (OnSameSide(a, b, c, d) || OnSameSide(c, d, a, b)) 
        return false;
    return true;
}

// Returns a tuple of (a, b, c) where a*x + b*y + c = 0 is the line's implicit equation.
tuple<T, T, T> LineEquation(const Point& a, const Point& b) {
    assert(a != b);
    Point ort = (b - a).rotateCCW90();
    T c = -cross(a, b);
    return make_tuple(ort.x, ort.y, c);
}

vector<Point> LineIntersection(const Point& a, const Point& b, const Point& c, const Point& d) {
    if(LinesParallel(a, b, c, d) || a == b || c == d)
        return {};
    Point ab = b - a; 
    Point dc = c - d; 
    Point ac = c - a;
    return {a + ab * cross(ac, dc) / cross(ab, dc)};
}


vector<Point> LineCircleIntersection(const Point& a, const Point& b, const Point& c, const T& r) {
    Point ab = b - a;
    ab.normalize();
    Point ac = c - a;
    T distCenterLine = cross(ab, ac);
    Point lineMid = c + ab.rotateCCW90() * distCenterLine;
    if (r < distCenterLine) {
        return {};
    }
    T expandFromMid = sqrt(r * r - ac.sqrlen());
    if (expandFromMid > EPS) {
        return {lineMid + ab * expandFromMid, lineMid - ab * expandFromMid};
    }
    return {lineMid};
}

vector<Point> CircleIntersection(const Point& a, T r, const Point& b, T R) {
    Point ab = b - a;
    T d = ab.len();
    if (d > r + R || d + min(r, R) <= max(r, R)){
        return {};
    }
    T aHeight = (d * d - R * R + r * r) / (2 * d);
    T expandFromMid = sqrt(r * r - aHeight * aHeight);
    ab.normalize();
    Point lineMid = a + ab * aHeight;
    Point perpab = ab.rotateCCW90();
    if (expandFromMid > EPS) {
        return {lineMid + perpab * expandFromMid, lineMid - perpab * expandFromMid};
    }
    return {lineMid};
}

int main() {
    auto tup = LineEquation(Point(1, 0), Point(0, 1));
    double a, b, c;
    tie(a, b, c) = tup;
    cout << a << " " << b << " " << c << "\n";
}