#include <iostream>
#include <cstdlib>
#include <functional>
#include <fstream>

using namespace std;

// Treap class, effectively implementing a super-dyanmic array.
// Elements are indexed starting with 1.
// TODO: Better Iterator.
namespace treap {

int myRand() {
    return ((rand()&((1<<15)-1))<<15) + (rand()&((1<<15)-1));
}

template<typename T>
struct Node {
    T val;
    int prio, sub;
    bool sw;
    Node *left, *right, *father;
 
    Node(const T& val) : val(val), prio(myRand()), sub(1), sw(false), 
    left(NULL), right(NULL), father(NULL)   {}
 
    int leftSize() const {
        return (left == NULL ? 0 : left->sub);
    }
 
    void update() {
        sub = 1;
        if (left != NULL) {
            sub += left->sub;
            left->father = this;
        }
        if (right != NULL) {
            sub += right->sub;
            right->father = this;
        }
    }
 
    void lazy() {
        if (sw) {
            swap(left, right);
            if (left != NULL)
                left->sw ^= 1;
            if (right != NULL)
                right->sw ^= 1;
            sw = false;
        }
    }
};

template<typename T>
class Iterator {
    Node<T>* current;
    int rnk;

  public: 
    Iterator(Node<T>* current=NULL, int rnk=0) : current(current), rnk(rnk) {}

    void operator ++() {
        if (current->right != NULL) {
            current = current->right;
            current->lazy();
            while (current->left != NULL) {
                current = current->left;
                current->lazy();
            }
        } else {
            Node<T>* last = current;
            current = current->father;
            while (current != NULL) {
                if (last == current->left) {
                    break;
                }
                last = current;
                current = current->father;
            }
        }
        ++rnk;
    }

    void operator --() {
        if (current->left != NULL) {
            current = current->left;
            current->lazy();
            while (current->right != NULL) {
                current = current->right;
                current->lazy();
            }
        } else {
            Node<T>* last = current;
            current = current->father;
            while(current != NULL) {
                if (last == current->right) {
                    break;
                }
                last = current;
                current = current->father;
            }
        }
        --rnk;
    }

    int rank() {
        return rnk;
    }

    bool invalid() {
        return current == NULL;
    }

    T value() {
        return current->val;
    }
};

template<typename T>
class Treap {
    Node<T>* root;
    int sz;
    int aux;
     
    // First k-1 nodes in L, everything else in R.
    void splitAtKth(Node<T>* C, Node<T>*& L, Node<T>*& R, int k) {
        if (C == NULL) {
            L = NULL;
            R = NULL;
            return;
        }
     
        C->lazy();
     
        if (C->leftSize() + 1 < k) {
            L = C;
            splitAtKth(C->right, L->right, R, k - C->leftSize() - 1);
            L->update();
        } else {
            R = C;
            splitAtKth(C->left, L, R->left, k);
            R->update();
        }
    }
    
    void insertAtKth(Node<T>*& C, Node<T>* N, int k) {
        if (C == NULL) {
            C = N;
            return;
        }
     
        C->lazy();
     
        if (N->prio < C->prio) {
            Node<T> *L, *R;
            splitAtKth(C, L, R, k);
            C = N;
            C->left = L;
            C->right = R;
        } else if (C->leftSize() >= k-1) {
            insertAtKth(C->left, N, k);
        } else {
            insertAtKth(C->right, N, k - C->leftSize() - 1);
        }

        C->update();
    }

    // All nodes <= val in L, everything else in R
    void splitByComparison(Node<T>* C, Node<T>*& L, Node<T>*& R, const T& val) {
        if (C == NULL) {
            L = NULL;
            R = NULL;
            return;
        }

        C->lazy();

        if (C->val <= val) {
            L = C;
            splitByComparison(C->right, L->right, R, val);
            L->update();
        } else {
            R = C;
            splitByComparison(C->left, L, R->left, val);
            R->update();
        }
    }

    void insertByComparison(Node<T>*& C, Node<T>* N) {
        if (C == NULL) {
            C = N;
            return;
        }

        C->lazy();
     
        if (N->prio < C->prio) {
            Node<T> *L, *R;
            splitByComparison(C, L, R, N->val);
            C = N;
            C->left = L;
            C->right = R;
        } else if (N->val <= C->val) {
            insertByComparison(C->left, N);
        } else {
            insertByComparison(C->right, N);
        }

        C->update();
    }

    void join(Node<T>*& C, Node<T>* L, Node<T>* R) {
        if (L == NULL) {
            C = R;
            return;
        }
        if (R == NULL) {
            C = L;
            return;
        }
     
        L->lazy();
        R->lazy();
     
        if(L->prio < R->prio) {
            C = L;
            join(C->right, L->right, R);
            C->update();
        } else {
            C = R;
            join(C->left, L, R->left);
            C->update();
        }
    }
     
    void eraseKth(Node<T>*& C, int k) {
        if (C == NULL)
            return;
        C->lazy();
        if (C->leftSize() + 1 == k) {
            Node<T> *L = C->left, *R = C->right;
            delete C;
            C = NULL;
            join(C, L, R);
            --sz;
        } else if (C->leftSize() >= k-1) {
            eraseKth(C->left, k);
        } else {
            eraseKth(C->right, k - C->leftSize() - 1);
        }
        if (C != NULL)
            C->update();
    }

    void eraseByComparison(Node<T>*& C, const T& val) {
        if (C == NULL)
            return;
        C->lazy();
        if (C->val == val) {
            Node<T> *L = C->left, *R = C->right;
            delete C;
            C = NULL;
            join(C, L, R);
            --sz;
        } else if (val <= C->val) {
            eraseByComparison(C->left, val);
        } else {
            eraseByComparison(C->right, val);
        }
        if (C != NULL)
            C->update();
    }
     
    Iterator<T> getKth(Node<T>* C, int k) {
        if (C == NULL)
            return Iterator<T>(NULL, aux);
        C->lazy();
     
        if (C->leftSize() + 1 == k) {
            return Iterator<T>(C, aux);
        } else if (C->leftSize() >= k-1) {
            return getKth(C->left, k);
        } else {
            return getKth(C->right, k - C->leftSize() - 1);
        }
    }

    Iterator<T> firstSatisfying(Node<T>* C, const T& val, 
        const function<bool (const T&, const T&)>& comp) {
        if (C == NULL) {
            return Iterator<T>(NULL, sz+1);
        }

        C->lazy();

        if (comp(val, C->val)) {
            auto it = firstSatisfying(C->left, val, comp);
            if (it.invalid()) {
                it = Iterator<T>(C, aux + 1);
            }
            return it;
        } else {
            aux += C->leftSize() + 1;
            return firstSatisfying(C->right, val, comp);
        }
    }

  public:

    Treap() : root(NULL), sz(0) {}

    void destroy(Node<T>* N) {
        if (N == NULL)
            return;
        destroy(N->left);
        destroy(N->right);
        delete N;
    }

    ~Treap() {
        destroy(root);
    }

    // Force insert at position k.
    void insertAt(int k, const T& val) {
        Node<T> *N = new Node<T>(val);
        insertAtKth(root, N, k);
        ++sz;
    }

    // Insert by using comparisons.
    // Needs operator <= defined on T.
    void insert(const T& val) {
        Node<T> *N = new Node<T>(val);
        insertByComparison(root, N);
        ++sz;
    }

    // Erases the kth element.
    bool eraseAt(int k) {
        int oldSz = sz;
        eraseKth(root, k);
        if (root != NULL) {
            root->father = NULL;
        }
        return sz < oldSz;
    }

    // Erases one instance of val.
    // Needs operator == and <= defined on T.
    bool erase(const T& val) {
        int oldSz = sz;
        eraseByComparison(root, val);
        if (root != NULL) {
            root->father = NULL;
        }
        return sz < oldSz;
    }

    // Returns an iterator to the kth element.
    Iterator<T> find(int k) {
        aux = k;
        return getKth(root, k);
    }

    // Returns iterator to the first element.
    Iterator<T> first() {
        return find(1);
    }

    // Returns iterator to the last element.
    Iterator<T> last() {
        return find(sz);
    }

    // Returns iterator to first element >= val.
    // Requires operator <= defined on T.
    Iterator<T> lowerBound(const T& val) {
        aux = 0;
        return firstSatisfying(root, val, [](const T& a, const T& b) {
            return a <= b;
        });
    }

    // Returns iterator to first element > val.
    // Requires operator < defined on T.
    Iterator<T> upperBound(const T& val) {
        aux = 0;
        return firstSatisfying(root, val, [](const T& a, const T& b) {
            return a < b;
        });
    }

    void reverse(int i, int j) {
        Node<T> *T12, *T1, *T2, *T3;
        splitAtKth(root, T12, T3, j+1);
        splitAtKth(T12, T1, T2, i);
        if (T2 != NULL) {
            T2->sw ^= 1;
        }
        join(T12, T1, T2);
        join(root, T12, T3);
    }

    int size() const {
        return sz;
    }
};

}

int main() {
    ifstream fin("secv8.in");
    ofstream fout("secv8.out");
    treap::Treap<int> t;
    
    int n, op;
    fin >> n >> op;

    for (int i = 1; i <= n; ++i) {
        char op;
        fin >> op;
        if (op == 'I') {
            int k, e;
            fin >> k >> e;
            t.insertAt(k, e);
        } else if (op == 'A') {
            int k;
            fin >> k;
            fout << t.find(k).value() << "\n";
        } else if (op == 'R') {
            int l, r;
            fin >> l >> r;
            t.reverse(l, r);
        } else {
            int l, r;
            fin >> l >> r;
            for (int j = l; j <= r; ++j) {
                t.eraseAt(l);
            }
        }
    }

    for (auto it = t.first(); !it.invalid(); ++it) {
        fout << it.value() << " ";
    }
}