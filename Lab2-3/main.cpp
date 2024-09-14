#include <iostream>
#include <memory>
#include <algorithm>
#include <cstring>
#include <functional>
#include <string>
#include <fstream>

#define LOG(x)

#pragma region Utils

#pragma region Vector

template<class T>
class TVector {
protected:
    T* data = nullptr;
    size_t size = 0, capacity = 0;

public:
    TVector() = default;
    ~TVector() {
        if (data != nullptr) {
            delete[] data;
        }
    }

    TVector(size_t _s) : size(_s), capacity(_s) {
        data = new T[_s];
    }

    TVector(size_t _s, const T& val) : size(_s), capacity(_s) {
        data = new T[_s];
        for (int i = 0; i < _s; i++) {
            data[i] = val;
        }
    }

    TVector(const std::initializer_list<T>& lst) {
        size_t n = lst.size();
        data = new T[n];
        size = n;
        capacity = n;
        size_t ptr = 0;
        for (const auto& el : lst) {
            data[ptr] = el;
            ptr++;
        }
    }

    TVector(const TVector& other) {
        if (data != nullptr) {
            delete[] data;
        }
        if (other.size == 0) {
            data = nullptr;
            size = 0;
            capacity = 0;
            return;
        }
        data = new T[other.size];
        size = other.size;
        capacity = other.size;
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    TVector(TVector&& other) {
        data = other.data;
        capacity = other.capacity;
        size = other.size;
        other.data = nullptr;
        capacity = 0;
        size = 0;
    }


    TVector& operator=(const TVector& other) {
        if (data != nullptr) {
            delete[] data;
        }
        if (other.data == nullptr) {
            data = nullptr;
            size = 0;
            capacity = 0;
            return *this;
        }
        data = new T[other.size];
        size = other.size;
        capacity = other.size;
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    TVector& operator=(TVector&& other) {
        std::cout << "Move assign\n";
        data = other.data;
        capacity = other.capacity;
        size = other.size;
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
        return *this;
    }


    size_t Size() const {
        return size;
    }

    void Resize(size_t nSize) {
        if (nSize < capacity) {
            size = nSize;
        } else {
            T* temp = new T[nSize];
            std::copy(data, data + size, temp);
            delete[] data;
            data = temp;
            size = nSize;
            capacity = nSize;
        }
    }

    void Resize(size_t nSize, const T& val) {
        Resize(nSize);
        for (int i = 0; i < nSize; i++) {
            data[i] = val;
        }
    }

    void PushBack(const T& el) {
        if (capacity == 0) {
            data = new T[1];
            size = 1;
            capacity = 1;
            data[0] = el;
        } else if (size < capacity) {
            data[size] = el;
            size++;
        } else {
            T* temp = new T[capacity * 2];
            capacity = capacity * 2;
            for (int i = 0; i < size; i++) {
                temp[i] = data[i];
            }
            delete[] data;
            temp[size] = el;
            size++;
            data = temp;
        }
    }

    T& operator[](size_t ind) {
        if (ind >= size) {
            throw std::out_of_range("Vector index out of range");
        }
        return data[ind];
    }

    const T& operator[](size_t ind) const {
        if (ind >= size) {
            throw std::out_of_range("Vector index out of range");
        }
        return data[ind];
    }

};

#pragma endregion Vector

#pragma region String

class TString : public TVector<char> {
public:
    using TVector<char>::TVector;

    ~TString() {
        
    }

    TString(const char* cStr) {
        int ptr = 0;
        data = nullptr;
        size = 0;
        capacity = 0;
        while (cStr[ptr] != '\0') {
            PushBack(cStr[ptr]);
            ptr++;
        }
    }

    std::unique_ptr<char[]> CStr() const {
        std::unique_ptr<char[]> res = std::make_unique<char[]>(Size() + 1);
        for (int i = 0; i < Size(); i++) {
            res[i] = data[i];
        }
        res[Size()] = '\0';
        return res;
    }

};

#pragma endregion String

bool operator<(const TString& lhs, const TString& rhs) {
    if (lhs.Size() == rhs.Size()) {
        for (int i = 0; i < lhs.Size(); i++) {
            if (lhs[i] < rhs[i]) {
                return true;
            } else if (lhs[i] > rhs[i]) {
                return false;
            }
        }
        return false;
    }
    return lhs.Size() < rhs.Size();
}

bool operator>(const TString& lhs, const TString& rhs) {
    return rhs < lhs;
}

bool operator==(const TString& lhs, const TString& rhs) {
    if (lhs.Size() != rhs.Size()) {
        return false;
    }
    for (int i = 0; i < lhs.Size(); i++) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

#pragma region TTreap

#include <random>

template<class K, class V>
class TTreap {
private:
    std::random_device dev;
    std::mt19937 rng;

    struct Node {
        K key;
        V value;
        Node *left = nullptr, *right = nullptr;
        int prior;

        Node(K k, V v, std::mt19937& rgen) : key(k), value(v) { prior = rgen(); }
        Node(K k, V v, int _p) : key(k), value(v), prior(_p) { }
    };
    using NodePair = std::pair<Node*, Node*>;

    static Node* Merge(Node *l, Node *r) {
        if (!l) return r;
        if (!r) return l;
        if (l->prior > r->prior) {
            l->right = Merge(l->right, r);
            return l;
        }
        else {
            r->left = Merge(l, r->left);
            return r;
        }
    }

    static NodePair Split (Node *p, K x) {
        if (!p) return {0, 0};
        if (p->key < x || p->key == x) {
            NodePair q = Split(p->right, x);
            p->right = q.first;
            return {p, q.second};
        }
        else {
            NodePair q = Split(p->left, x);
            p->left = q.second;
            return {q.first, p};
        }
    }

    Node* root = nullptr;

public:
    TTreap() : rng(dev()) { }

    ~TTreap() {
        std::function<void(Node*)> RecImpl = [&RecImpl](Node* node) -> void {
            if (node != nullptr) {
                RecImpl(node->left);
                RecImpl(node->right);
                delete node;
            }
        };

        RecImpl(root);
    }

    TTreap(const TTreap& other) {
        std::function<Node*(Node*)> Copy = [&Copy](Node* node) -> Node* {
            if (node == nullptr) {
                return nullptr;
            }
            Node *nNode = new Node(node->key, node->value, node->prior);
            nNode->left = Copy(node->left);
            nNode->right = Copy(node->right);
            return nNode;
        };
        root = Copy(other.root);
    }

    static void Swap(TTreap& lhs, TTreap& rhs) {
        std::swap(lhs.root, rhs.root);
    }

    TTreap(TTreap&& other) {
        root = nullptr;
        Swap(*this, other);
        std::cout << "TTreap moved\n";
    }

    TTreap& operator=(TTreap other) {
        TTreap tmp(other);
        Swap(*this, tmp);
        return *this;
    }


    V Get(const K& key) {
        std::function<Node*(Node*, const K&)> RecImpl = [&RecImpl](Node* n, const K& key) -> Node* {
            if (n == nullptr) {
                return nullptr;
            }
            if (n->key == key) {
                return n;
            } else if (key < n->key) {
                return RecImpl(n->left, key);
            }
            return RecImpl(n->right, key);
        };

        Node* result = RecImpl(root, key);
        if (result == nullptr) {
            throw std::out_of_range("No such key in treap");
        }
        return result->value;
    }

    bool Search(const K& key) {
        try {
            Get(key);
            return true;
        } catch (const std::out_of_range& ex) {
            return false;
        }
    }

    bool Insert(const K& key, const V& value) {
        if (Search(key)) {
            return false;
        }
        NodePair p = Split(root, key);
        Node *nNode = new Node(key, value, rng);
        root = Merge(p.first, Merge(nNode, p.second));
        return true;
    }

    void Erase(const K& key) {
        std::function<Node*(Node*, const K&)> RecImpl = [&RecImpl, this](Node* node, const K& key) -> Node* {
            if (node == nullptr) {
                return nullptr;
            }
            if (node->key == key) {
                Node* tmp = Merge(node->left, node->right);
                delete node;
                return tmp;
            } else if (node->key < key) {
                Node* r = RecImpl(node->right, key);
                node->right = r;
                return node;
            }
            Node* l = RecImpl(node->left, key);
            node->left = l;
            return node;
        };

        root = RecImpl(root, key);
    }

};

#pragma endregion TTreap

#pragma endregion Utils

#pragma region UI

TVector<TString> Split(const TString& str) {
    TVector<TString> result;

    enum class State {
        START,
        IN_WORD,
        IN_DELIMETER
    };

    auto isDel = [](const char& c) -> bool {
        return c == ' '  ||
               c == '\t';
    };

    State state = State::START;

    for (int i = 0; i < str.Size(); i++) {
        if (isDel(str[i])) {
            state = State::IN_DELIMETER;
        } else {
            if (state == State::START || state == State::IN_DELIMETER) {
                char s[2]; s[0] = str[i]; s[1] = '\0';
                result.PushBack(TString(s));
                state = State::IN_WORD;
            } else if (state == State::IN_WORD) {
                result[result.Size() - 1].PushBack(str[i]);
            }
        }
    }

    return result;
}

TString GetLine(bool& eofFlag) {
    char c;
    TString result;
    while (std::cin) {
        std::cin.read(&c, 1);
        if (std::cin.fail()) {
            eofFlag = true;
            return result;
        }
        if (c == '\n') {
            eofFlag = false;
            return result;
        }
        result.PushBack(c);
    }
    return result;
}

uint64_t StrToUint64(const TString& str) {
    uint64_t curpow = 1, result = 0;
    for (int i = str.Size() - 1; i >= 0; i--) {
        result += curpow * (str[i] - '0');
        curpow *= 10;
    }
    return result;
}

void ToLower(TString& str) {
    for (int i = 0; i < str.Size(); i++) {
        if (isalpha(str[i])) {
            str[i] = tolower(str[i]);
        }
    }
}

void Handle(TVector<TString>& args, TTreap<TString, uint64_t>& tree) {
    if (args.Size() == 3) {
        // + word int
        ToLower(args[1]);
        if (args[0][0] == '+') {
            if (tree.Search(args[1])) {
                std::cout << "Exist\n";
            } else {
                tree.Insert(args[1], StrToUint64(args[2]));
                std::cout << "OK\n";
            }
        }
    } else if (args.Size() == 2) {
        // - word
        ToLower(args[1]);
        if (args[0][0] == '-') {
            if (!tree.Search(args[1])) {
                std::cout << "NoSuchWord\n";
            } else {
                std::cout << "OK\n";
                tree.Erase(args[1]);
            }
        }
    } else if (args.Size() == 1) {
        ToLower(args[0]);
        if (tree.Search(args[0])) {
            std::cout << "OK: " << tree.Get(args[0]) << std::endl;
        } else {
            std::cout << "NoSuchWord\n";
        }
    }
}

void Start() {
    bool eof = false;
    TString str;
    TTreap<TString, uint64_t> tree;

    while (!eof) {
        str = GetLine(eof);
        TVector<TString> args = Split(str);
        try {
            Handle(args, tree);
        } catch (const std::exception& ex) {
            std::cout << "ERROR: " << ex.what() << std::endl;
        }
    }
}

#pragma endregion UI

#include <fstream>

int main() {
    Start();
}
