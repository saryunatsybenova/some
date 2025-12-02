//
// Created by sphdx on 6/22/25.
//

#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include "../list/List.h"

template <typename T>
class AVLTree {
    struct Node {
        T key;
        mutable int height;
        Node *left;
        Node *right;
        SLList<int> list;

        explicit Node(const T key) {
            this->key = key;
            left = right = nullptr;
            height = 1;
        }
    };

    Node *root = nullptr;
    int nodes_count = 0;

    static int get_height(const Node *node);
    static int balance_factor(const Node *node);
    static void update_height(const Node *node);
    static Node* right_rotate(Node *node);
    static Node* left_rotate(Node *node);
    static Node* balance(Node *node);
    Node* insert(Node *node, T key, int id);

    template<typename Callback>
    Node* insert(Node *node, T key, int id, Callback&& visit);
    static Node* find_min_node(Node *node);
    static Node* find_max_node(Node *node);
    static Node* delete_min_node(Node *node);
    static Node* delete_max_node(Node *node);
    static Node* delete_node(Node *node, T key, int id);
    static Node* delete_node(Node *node, T key);
    static void get_tree_in_order(const Node *node, int row, int col, int height, std::vector<std::vector<T>> &ans);
    static std::vector<std::vector<T>> tree_to_matrix(Node *node);
    static std::string structure(Node *node);
    static void print_pre_order(Node *node);
    static void print_in_order(Node *node);
    static void fill_keys_in_order(Node *node, T* keys, size_t& index);
    static void print_post_order(Node *node);
    static void print_reverse_in_order(Node *node);
    void clear_tree(Node *node);

    template<typename Callback>
    static Node * search_node(Node *node, T key, Callback&& visit);
    template<typename Callback>
    static Node * search_node(Node *node, T key, int id, Callback&& visit);

    void inorder_to_array(Node* node, T* key_arr, SLList<int>* list_arr, int& index, int arr_length) const;
    void lying_tree(Node *node, std::ostringstream &oss, size_t space = 0) const;
    template<typename Callback>
    void range_search(Node* node, T& low, T& high, Callback&& visit) const;

public:
    [[nodiscard]] std::string structure() const;
    [[nodiscard]] std::string lying_tree() const;
    void insert(T key, int id);

    template <typename Callback>
    void insert(T key, int id, Callback&& visit);

    void update(T key, const int *ids, size_t count);
    void replace(T key, int id_to_replace, int new_id);
    void del(T key, int id);
    void del(T key);
    void print_pre_order() const;
    void print_in_order() const;
    T* keys_in_order() const;
    void print_post_order() const;
    void print_reverse_in_order() const;
    void clear();

    template<class Callback>
    const Node * search(T key, int id, Callback &&visit) const;

    template<typename Callback>
    const Node * search(T key, Callback &&visit) const;

    template<typename Callback>
    void range_search(T& low, T& high, Callback&& visit) const;

    [[nodiscard]] int get_nodes_count() const;
    bool operator==(const AVLTree &other) const;

    template<typename Callback>
    int *to_arr(T key, size_t &count, Callback &&visit);
    [[nodiscard]] size_t list_size(T key);
};

template <typename T>
int AVLTree<T>::get_height(const Node *node) {
    return node ? node->height : 0;
}

template <typename T>
 int AVLTree<T>::balance_factor(const Node *node) {
    return get_height(node->right) - get_height(node->left);
}

template <typename T>
 void AVLTree<T>::update_height(const Node *node) {
    const int hl = get_height(node->left);
    const int hr = get_height(node->right);

    node->height = (hl > hr ? hl : hr) + 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Правое вращение используется когда разница высот R поддерева и b-поддерева равна 2
// и высота C <= высота L
//
//       a              b
//    b     R   ->   L     a
//  L   C                C   R
//
// Алгоритм:
// Сохраняем указатель на элемент b. Вместо левого потомка у a помещаем указатель на
// правого потомка b.
//
//       a              a
//    b     R   ->   C     R
//
// В качестве правого потомка элемента b ставим указатель на элемент a
//
//       b
//    L     a
//        C   R
//
// Далее обновляем высоты для поддеревьев а и b
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::right_rotate(Node *node) {
    Slog::info("Правый поворот в дереве");
    Node* temp = node->left;
    node->left = temp->right;
    temp->right = node;

    update_height(node);
    update_height(temp);

    return temp;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Левое вращение используется когда разница высот L поддерева и b-поддерева равна 2
// и высота C <= высота R
//
//       a              b
//    L     b   ->   a     R
//        C   R    L   C
//
// Алгоритм:
// Сохраняем указатель на элемент b. Вместо правого потомка у a помещаем указатель на
// левого потомка b.
//
//       a              a
//    L     b   ->   L     C
//
// В качестве левого потомка элемента b ставим указатель на элемент a
//
//       b
//    a     R
//  L   C
//
// Далее обновляем высоты для поддеревьев а и b
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::left_rotate(Node *node) {
    Slog::info("Левый поворот в дереве");
    Node* temp = node->right;
    node->right = temp->left;
    temp->left = node;

    update_height(node);
    update_height(temp);

    return temp;
}

// Метод balance осуществляет левый-правый или правый-левый (большое левое, большое правое) повороты.
template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::balance(Node *node) {
    update_height(node);

    if (balance_factor(node) == 2) {
        Slog::info("Разница высот поддеревьев равна 2");
        if (balance_factor(node->right) < 0) {
            node->right = right_rotate(node->right);
        }
        return left_rotate(node);
    }
    if (balance_factor(node)== -2){
        Slog::info("Разница высот поддеревьев равна -2");
        if (balance_factor(node->left) > 0) {
            node->left = left_rotate(node->left);
        }
        return right_rotate(node);
    }

    return node; // балансировка не нужна
}

template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::insert(Node *node, const T key, int id) {
    if( !node ) { auto newNode = new Node(key); newNode->list.push_back(id); nodes_count += 1; return newNode; }

    /////////////////
    if (key == node->key) {
        node->list.push_back(id);
    }
    /////////////////


    if(key < node->key) {
        node->left = insert(node->left,key, id);
    }
    else if(key > node->key) {
        node->right = insert(node->right,key, id);
    }

    return balance(node);
}

template<typename T>
template<typename Callback>
typename AVLTree<T>::Node * AVLTree<T>::insert(Node *node, T key, int id, Callback &&visit) {
    if( !node ) { auto newNode = new Node(key); newNode->list.push_back(id); nodes_count += 1; visit(); return newNode; }

    /////////////////
    if (key == node->key) {
        node->list.push_back(id);
    }
    /////////////////


    if(key < node->key) {
        node->left = insert(node->left,key, id);
    }
    else if(key > node->key) {
        node->right = insert(node->right,key, id);
    }

    Slog::info("Балансировка дерева");
    return balance(node);
}

// find_min_node осуществляет поиск минимального узла относительно дерева, для которого переданный узел
// является корнем
template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::find_min_node(Node *node) {
    return node->left ? find_min_node(node->left) : node;
}

// find_max_node осуществляет поиск максимального узла относительно дерева, для которого переданный узел
// является корнем
template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::find_max_node(Node *node) {
    return node->right ? find_max_node(node->right) : node;
}

template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::delete_min_node(Node *node) {
    if (node->left == nullptr) {
        return node->right;
    }

    node->left = delete_min_node(node->left);

    return balance(node);
}

template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::delete_max_node(Node *node) {
    if (node->right == nullptr) {
        return node->left;
    }

    node->right = delete_max_node(node->right);

    return balance(node);
}

template <typename T>
typename AVLTree<T>::Node * AVLTree<T>::delete_node(Node *node, T key, int id) {
    if (!node) return nullptr;

    if (key < node->key)
        node->left = delete_node(node->left,key,id);
    else if (key > node->key)
        node->right = delete_node(node->right,key,id);
    else {
        if (node->list.search(id) != nullptr) {
            node->list.del(id);
        }

        if (node->list.count() == 0) {
            Node* leftNode = node->left;
            Node* rightNode = node->right;
            delete node;

            if (!leftNode) return rightNode;

            Node* max = find_max_node(leftNode);
            max->left = delete_max_node(leftNode);
            max->right = rightNode;

            return balance(max);
        }
    }

    return balance(node);
}

template<typename T>
typename AVLTree<T>::Node * AVLTree<T>::delete_node(Node *node, T key) {
    if (!node) return nullptr;

    if (key < node->key)
        node->left = delete_node(node->left,key);
    else if (key > node->key)
        node->right = delete_node(node->right,key);
    else {
        Node* leftNode = node->left;
        Node* rightNode = node->right;
        delete node;

        if (!leftNode) return rightNode;

        Node* max = find_max_node(leftNode);
        max->left = delete_max_node(leftNode);
        max->right = rightNode;

        return balance(max);
    }

    return balance(node);
}

template <typename T>
void AVLTree<T>::get_tree_in_order(const Node *node, const int row, const int col, const int height, std::vector<std::vector<T>> &ans) {
    if (!node) { return; }

    const int offset = pow(2, height - row - 1);

    if (node->left) {
        get_tree_in_order(node->left, row + 1, col - offset, height, ans);
    }

    ans[row][col] = node->key;

    if (node->right) {
        get_tree_in_order(node->right, row + 1, col + offset, height, ans);
    }
}

template <typename T>
std::vector<std::vector<T>> AVLTree<T>::tree_to_matrix(Node *node) {
    const int height = get_height(node);

    const int rows = height + 1;
    const int cols = pow(2, height + 1) - 1;

    std::vector ans(rows, std::vector<T>(cols));

    get_tree_in_order(node, 0, (cols - 1) / 2, height, ans);

    return ans;
}

template <typename T>
std::string AVLTree<T>::structure(Node *node) {
    std::ostringstream oss;

    for (auto result = tree_to_matrix(node); auto &row : result) {
        for (const auto &cell: row) {
            if (cell == T()) {
                oss << "\t\t";
            } else {
                oss << cell;
            }
        }
        oss << std::endl;
    }

    return oss.str();
}

template<typename T>
void AVLTree<T>::print_pre_order(Node *node) {
    if (node == nullptr) return;

    std::cout << node->key;
    node->list.print();

    printPreOrder(node->left == nullptr ? nullptr : node->left);
    printPreOrder(node->right == nullptr ? nullptr : node->right);
}

template<typename T>
void AVLTree<T>::print_in_order(Node *node) {
    if (node == nullptr) return;

    print_in_order(node->left == nullptr ? nullptr : node->left);

    std::cout << node->key << ",";
    // node->list.print();

    print_in_order(node->right == nullptr ? nullptr : node->right);
}

template<typename T>
void AVLTree<T>::fill_keys_in_order(Node *node, T *keys, size_t &index) {
    if (!node) return;
    fill_keys_in_order(node->left, keys, index);
    keys[index++] = node->key;
    fill_keys_in_order(node->right, keys, index);
}

template<typename T>
void AVLTree<T>::print_post_order(Node *node) {
    if (node == nullptr) return;

    print_post_order(node->left == nullptr ? nullptr : node->left);
    print_post_order(node->right == nullptr ? nullptr : node->right);

    std::cout << node->key;
    node->list.print();
}

template<typename T>
void AVLTree<T>::print_reverse_in_order(Node *node) {
    if (node == nullptr) return;

    print_reverse_in_order(node->right == nullptr ? nullptr : node->right);

    std::cout << node->key;
    node->list.print();

    print_reverse_in_order(node->left == nullptr ? nullptr : node->left);
}

template<typename T>
void AVLTree<T>::clear_tree(Node *node) {
    if (node == nullptr) return;
    clear_tree(node->left == nullptr ? nullptr : node->left);
    clear_tree(node->right == nullptr ? nullptr : node->right);
    delete node;
}

template<typename T>
template<typename Callback>
typename AVLTree<T>::Node * AVLTree<T>::search_node(Node *node, T key, Callback &&visit) {
    visit();
    if (node == nullptr || node->key == key) return node;
    if (key < node->key)
        return search_node(node->left, key, visit);
    return search_node(node->right, key, visit);
}

template<typename T>
template<typename Callback>
typename AVLTree<T>::Node * AVLTree<T>::search_node(Node *node, T key, int id, Callback &&visit) {
    visit();
    if (node == nullptr || node->key == key && node->list.count(id) > 0) return node;
    if (key < node->key)
        return search_node(node->left, key, id, visit);
    return search_node(node->right, key, id, visit);
}

// structure печатает дерево полностью, игнорируя нулевые значения переданного типа
template<typename T>
std::string AVLTree<T>::structure() const {
    // return structure(this->root);
    const Node *root = this->root;
    if (!root) return {};

    auto decode = [](const std::string &s)-> std::u32string {
        std::u32string out;
        for (size_t i = 0; i < s.size();) {
            unsigned char c = s[i];
            char32_t cp = 0;
            if ((c & 0x80) == 0) {
                cp = c;
                i += 1;
            } else if ((c & 0xE0) == 0xC0) {
                cp = (c & 0x1F) << 6
                     | (s[i + 1] & 0x3F);
                i += 2;
            } else if ((c & 0xF0) == 0xE0) {
                cp = (c & 0x0F) << 12
                     | (s[i + 1] & 0x3F) << 6
                     | (s[i + 2] & 0x3F);
                i += 3;
            } else {
                cp = (c & 0x07) << 18
                     | (s[i + 1] & 0x3F) << 12
                     | (s[i + 2] & 0x3F) << 6
                     | (s[i + 3] & 0x3F);
                i += 4;
            }
            out.push_back(cp);
        }
        return out;
    };
    auto encode = [](char32_t cp)-> std::string {
        std::string o;
        if (cp <= 0x7F) o.push_back(char(cp));
        else if (cp <= 0x7FF) {
            o.push_back(char(0xC0 | (cp >> 6)));
            o.push_back(char(0x80 | (cp & 0x3F)));
        } else if (cp <= 0xFFFF) {
            o.push_back(char(0xE0 | (cp >> 12)));
            o.push_back(char(0x80 | ((cp >> 6) & 0x3F)));
            o.push_back(char(0x80 | (cp & 0x3F)));
        } else {
            o.push_back(char(0xF0 | (cp >> 18)));
            o.push_back(char(0x80 | ((cp >> 12) & 0x3F)));
            o.push_back(char(0x80 | ((cp >> 6) & 0x3F)));
            o.push_back(char(0x80 | (cp & 0x3F)));
        }
        return o;
    };

    auto height = [&](auto &&self, const Node *n)-> int {
        return n ? 1 + std::max(self(self, n->left), self(self, n->right)) : -1;
    };
    const int H = height(height, root);

    auto max_cols = [&](auto &&self, const Node *n)-> int {
        if (!n) return 0;
        std::ostringstream tmp;
        tmp << n->key;
        int here = static_cast<int>(decode(tmp.str()).size());
        return std::max({here, self(self, n->left), self(self, n->right)});
    };
    const int KEY_W = max_cols(max_cols, root);

    const int CELL_W = KEY_W + 2;
    const int BOX_W = CELL_W + 2;
    const int GAP = 2;
    const int COLS = (1 << (H + 1)) - 1;
    const int ROWS = H + 1;
    const int CH = ROWS * 4 - 1;
    const int CW = COLS * (BOX_W + GAP);

    std::vector canvas(CH, std::u32string(CW, U' '));
    auto cx = [&](int col) { return col * (BOX_W + GAP) + BOX_W / 2; };

    auto place = [&](auto &&self, const Node *n, int row, int col, int level)-> void {
        if (!n) return;
        int x0 = col * (BOX_W + GAP);
        int y0 = row * 4;

        std::ostringstream oss;
        oss << n->key;
        std::u32string key32 = decode(oss.str());
        int pad = CELL_W - int(key32.size());
        std::u32string mid = U"|";
        mid.append(pad / 2, U' ');
        mid += key32;
        mid.append(pad - pad / 2, U' ');
        mid += U"|";

        canvas[y0].replace(x0, BOX_W, std::u32string(U"+") + std::u32string(CELL_W, U'-') + U"+");
        canvas[y0 + 1].replace(x0, BOX_W, mid);
        canvas[y0 + 2].replace(x0, BOX_W, std::u32string(U"+") + std::u32string(CELL_W, U'-') + U"+");

        if (!n->left && !n->right) return;

        int midx = cx(col);
        canvas[y0 + 3][midx] = U'|';
        int offset = 1 << (level - row - 1);

        if (n->left) {
            int cl = col - offset, xl = cx(cl);
            for (int x = xl; x < midx; ++x) canvas[y0 + 3][x] = U'-';
            canvas[y0 + 3][xl] = U'+';
            self(self, n->left, row + 1, cl, level);
        }
        if (n->right) {
            int cr = col + offset, xr = cx(cr);
            for (int x = midx + 1; x <= xr; ++x) canvas[y0 + 3][x] = U'-';
            canvas[y0 + 3][xr] = U'+';
            self(self, n->right, row + 1, cr, level);
        }
    };
    place(place, root, 0, (COLS - 1) / 2, H);

    std::ostringstream out;
    for (const auto &row: canvas) {
        for (char32_t cp: row) out << encode(cp);
        out << '\n';
    }
    return out.str();
}

template<typename T>
std::string AVLTree<T>::lying_tree() const {
    std::ostringstream oss;
    lying_tree(this->root, oss);
    return oss.str();
}

template<typename T>
void AVLTree<T>::insert(const T key, const int id) {
    this->root = insert(this->root, key, id);
}

template<typename T>
template<typename Callback>
void AVLTree<T>::insert(T key, int id, Callback &&visit) {
    this->root = insert(this->root, key, id, visit);
}

template<typename T>
void AVLTree<T>::update(T key, const int *ids, const size_t count) {
    auto *node = search(key);
    if (node == nullptr) return;

    SLList<int> new_list;

    for (size_t i = 0; i < count; ++i) {
        new_list.push_back(ids[i]);
    }

    node->list = new_list;
}

template<typename T>
void AVLTree<T>::replace(T key, int id_to_replace, int new_id) {
    auto *node = search_node(this->root, key, []{});
    if (node == nullptr) return;

    if (node->list.count(id_to_replace) == 0) return;
    node->list.del(id_to_replace);
    node->list.add(new_id);
}

template<typename T>
void AVLTree<T>::del(const T key, const int id) {
    this->root = delete_node(this->root, key, id);
}

template<typename T>
void AVLTree<T>::del(T key) {
    this->root = delete_node(this->root, key);
}

// print_pre_order осуществляет прямой обход дерева
template<typename T>
void AVLTree<T>::print_pre_order() const {
    print_pre_order(this->root);
}

// print_in_order осуществляет центрированный обход дерева
// такой обход выводит элементы в отсортированном порядке
template<typename T>
void AVLTree<T>::print_in_order() const {
    print_in_order(this->root);
}

template<typename T>
T *AVLTree<T>::keys_in_order() const {
    if (this->root == nullptr)
        return nullptr;

    T *keys = new T[nodes_count];
    size_t index = 0;
    fill_keys_in_order(this->root, keys, index);

    return keys;
}

// print_post_order осуществляет обратный обход дерева
template<typename T>
void AVLTree<T>::print_post_order() const {
    print_post_order(this->root);
}

// print_reverse_in_order осуществляет центрированный обход дерева в обратном порядке
template<typename T>
void AVLTree<T>::print_reverse_in_order() const {
    print_reverse_in_order(this->root);
}

template<typename T>
void AVLTree<T>::clear() {
    if (this->root == nullptr) { return; }
    clear_tree(this->root->left);
    clear_tree(this->root->right);
    delete this->root;
    this->root = nullptr;
}

template<typename T>
template<typename Callback>
const typename AVLTree<T>::Node* AVLTree<T>::search(T key, const int id, Callback &&visit) const {
    return search_node(this->root, key, id, visit);
}

template<typename T>
template<typename Callback>
const typename AVLTree<T>::Node * AVLTree<T>::search(T key, Callback &&visit) const {
    return search_node(this->root, key, visit);
}

template<typename T>
template<typename Callback>
void AVLTree<T>::range_search(T &low, T &high, Callback &&visit) const {
    return range_search(this->root, low, high, visit);
}

template<typename T>
int AVLTree<T>::get_nodes_count() const {
    return nodes_count;
}

template<typename T>
void AVLTree<T>::inorder_to_array(Node *node, T *key_arr, SLList<int> *list_arr, int &index,
                                  const int arr_length) const {
    if (node == nullptr || index >= arr_length) {
        return;
    }

    inorder_to_array(node->left, key_arr, list_arr, index, arr_length);

    if (index < arr_length) {
        key_arr[index] = node->key;
        list_arr[index] = node->list;
        index++;
    }

    inorder_to_array(node->right, key_arr, list_arr, index, arr_length);
}

template<typename T>
void AVLTree<T>::lying_tree(Node *node, std::ostringstream &oss, size_t space) const {
    if (node == nullptr)
        return;

    space += 2;
    lying_tree(node->right, oss, space);
    for (int i = 2; i < space; ++i)
        oss << "  ";
    oss << node->key << ": ";
    oss << node->list.structure();

    lying_tree(node->left, oss, space);
}

template<typename T>
template<typename Callback>
void AVLTree<T>::range_search(Node *node, T &low, T &high, Callback &&visit) const {
    if (node == nullptr) return;

    if (low < node->key)
        range_search(node->left, low, high, visit);

    if (low <= node->key && node->key <= high)
        visit(node->list);

    if (node->key < high)
        range_search(node->right, low, high, visit);
}

template<typename T>
bool AVLTree<T>::operator==(const AVLTree &other) const {
    const int size1 = this->get_nodes_count();
    const int size2 = other.get_nodes_count();

    if (size1 != size2) {
        return false;
    }

    T* keyArr1 = new T[size1];
    auto* listArr1 = new SLList<int>[size1];
    T* keyArr2 = new T[size2];
    auto* listArr2 = new SLList<int>[size2];

    int index1 = 0, index2 = 0;

    inorder_to_array(this->root, keyArr1, listArr1, index1, size1);

    inorder_to_array(other.root, keyArr2, listArr2, index2, size2);

    for (int i = 0; i < size1; ++i) {
        if (keyArr1[i] != keyArr2[i] || listArr1[i] != listArr2[i]) {
            delete[] keyArr1;
            delete[] listArr1;
            delete[] keyArr2;
            delete[] listArr2;
            return false;
        }
    }

    delete[] keyArr1;
    delete[] listArr1;
    delete[] keyArr2;
    delete[] listArr2;

    return true;
}

template<typename T>
template<typename Callback>
int* AVLTree<T>::to_arr(T key, size_t &count, Callback&& visit) {
    auto *node = search(key, visit);
    if (node == nullptr) {
        count = 0;
        return nullptr;
    }

    count = node->list.count();
    if (count == 0) {
        return nullptr;
    }

    auto ids = new int[count];
    for (int i = 0; i < count; ++i) {
        ids[i] = node->list[i];
    }

    return ids;
}

template<typename T>
size_t AVLTree<T>::list_size(T key) {
    auto *node = search(key);
    if (node == nullptr) return 0;
    return node->list.count();
}

#endif //AVLTREE_H
