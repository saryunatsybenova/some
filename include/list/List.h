#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <ostream>
#include <stdexcept>

// Данный модуль определяет двусвязный список совместимый с API SLList

template <typename T>
class List {
public:
    class Node {
    public:
        explicit Node(T data, Node* pNext = nullptr, Node* pPrev = nullptr) {
            this->pNext = pNext;
            this->pPrev = pPrev;
            this->data = data;
        }
        Node* pNext = nullptr;
        Node* pPrev = nullptr;
        T data = T();
    };

    List();
    ~List();

    void del(T data);
    void del_after(T data);
    void add(T data);
    void push_back(T data);
    Node* search(T data);
    void print();
    std::string structure() const;
    List* copy();
    int count(T data);
    [[nodiscard]] int count() const;
    T get_first();

    Node* head = nullptr;
    Node* tail = nullptr;

    bool operator==(const List<T> &other) const;
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    T& at(size_t index);
    const T& at(size_t index) const;

private:
    size_t size_ = 0;
};

template<typename T>
List<T>::List() = default;

template<typename T>
List<T>::~List() {
    while (head != nullptr) {
        Node* next = head->pNext;
        delete head;
        head = next;
    }
}

template<typename T>
void List<T>::del(T data) {
    Node* current = head;
    
    while (current != nullptr) {
        if (current->data == data) {
            // Обновляем указатели
            if (current->pPrev) {
                current->pPrev->pNext = current->pNext;
            } else {
                head = current->pNext;
            }
            
            if (current->pNext) {
                current->pNext->pPrev = current->pPrev;
            } else {
                tail = current->pPrev;
            }
            
            Node* next = current->pNext;
            delete current;
            current = next;
            size_--;
        } else {
            current = current->pNext;
        }
    }
}

template<typename T>
void List<T>::del_after(T data) {
    Node* current = search(data);
    if (current != nullptr && current->pNext != nullptr) {
        Node* toDelete = current->pNext;
        
        current->pNext = toDelete->pNext;
        if (toDelete->pNext) {
            toDelete->pNext->pPrev = current;
        } else {
            tail = current;
        }
        
        delete toDelete;
        size_--;
    }
}

template<typename T>
void List<T>::add(T data) {
    Node* new_node = new Node(data);
    
    if (head == nullptr) {
        head = tail = new_node;
    } else {
        tail->pNext = new_node;
        new_node->pPrev = tail;
        new_node->pNext = head;
        tail = new_node;
    }
    size_++;
}

template<typename T>
void List<T>::push_back(T data) {
    Node* new_node = new Node(data);
    
    if (head == nullptr) {
        head = tail = new_node;
    } else {
        tail->pNext = new_node;
        new_node->pPrev = tail;
        tail = new_node;
    }
    size_++;
}

template<typename T>
typename List<T>::Node* List<T>::search(T data) {
    Node* current = head;
    
    while (current != nullptr) {
        if (current->data == data) {
            return current;
        }
        current = current->pNext;
    }
    
    return nullptr;
}

template<typename T>
void List<T>::print() {
    if (head == nullptr) {
        std::cout << "[ ]" << '\n';
        return;
    }
    
    Node* current = head;
    std::cout << "[";
    
    while (current != nullptr) {
        std::cout << " " << current->data;
        current = current->pNext;
    }
    
    std::cout << " ]" << '\n';
}

template<typename T>
std::string List<T>::structure() const {
    std::ostringstream oss;
    if (head == nullptr) {
        oss << "[ ]" << '\n';
        return oss.str();
    }
    
    Node* current = head;
    oss << "[";
    
    while (current != nullptr) {
        oss << " " << current->data;
        current = current->pNext;
    }
    
    oss << " ]" << '\n';
    return oss.str();
}

template<typename T>
List<T>* List<T>::copy() {
    List<T>* new_list = new List<T>();
    
    Node* current = head;
    while (current != nullptr) {
        new_list->push_back(current->data);
        current = current->pNext;
    }
    
    return new_list;
}

template<typename T>
int List<T>::count(T data) {
    int counter = 0;
    Node* current = head;
    
    while (current != nullptr) {
        if (current->data == data) {
            counter++;
        }
        current = current->pNext;
    }
    
    return counter;
}

template<typename T>
int List<T>::count() const {
    return static_cast<int>(size_);
}

template<typename T>
T List<T>::get_first() {
    if (head == nullptr) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
bool List<T>::operator==(const List<T> &other) const {
    if (count() != other.count()) return false;
    
    Node* currentThis = head;
    Node* currentOther = other.head;
    
    while (currentThis != nullptr && currentOther != nullptr) {
        if (currentThis->data != currentOther->data) {
            return false;
        }
        currentThis = currentThis->pNext;
        currentOther = currentOther->pNext;
    }
    
    return currentThis == nullptr && currentOther == nullptr;
}

template<typename T>
T& List<T>::operator[](size_t index) {
    Node* current = head;
    for (size_t i = 0; i < index && current != nullptr; i++) {
        current = current->pNext;
    }
    
    if (current == nullptr) {
        throw std::out_of_range("List index out of range");
    }
    
    return current->data;
}

template<typename T>
const T& List<T>::operator[](size_t index) const {
    const Node* current = head;
    for (size_t i = 0; i < index && current != nullptr; i++) {
        current = current->pNext;
    }
    
    if (current == nullptr) {
        throw std::out_of_range("List index out of range");
    }
    
    return current->data;
}

template<typename T>
T& List<T>::at(size_t index) {
    if (head == nullptr) throw std::out_of_range("List::at");
    if (index >= size_) throw std::out_of_range("List::at");
    return (*this)[index];
}

template<typename T>
const T& List<T>::at(size_t index) const {
    if (head == nullptr) throw std::out_of_range("List::at");
    if (index >= size_) throw std::out_of_range("List::at");
    return (*this)[index];
}

// Алиас для совместимости с SLList
template <typename T>
using SLList = List<T>;

#endif //LIST_H