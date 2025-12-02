//
// Created by sphdx on 7/8/25.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class [[maybe_unused]] Vector {
public:
    using iterator = T *;
    using const_iterator = const T *;
    using alloc_traits = std::allocator_traits<Allocator>;

private:
    Allocator alloc_;
    T *data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

public:
    Vector();
    Vector(const Vector &other);
    Vector(size_t size, const T& value);
    explicit Vector(size_t size);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    void reserve(size_t new_capacity) noexcept;
    void resize(size_t n);
    void push_back(const T &value) noexcept;
    void push_back(T &&value) noexcept;
    void pop_back() noexcept;
    iterator del(const T& value);
    iterator insert(const_iterator pos, const T &value);

    Vector &operator=(const Vector &other);

    Vector &operator=(Vector &&other) noexcept;

    ~Vector();

    T &operator[](size_t index) noexcept;
    const T &operator[](size_t index) const noexcept;
    T &at(size_t index);
    const T &at(size_t index) const;
    T &front();
    const T &front() const;
    T &back();
    const T &back() const;
    iterator erase(const_iterator pos);
    void clear();

    iterator erase_swap(const_iterator pos);
    iterator remove_swap(const T &value);
};

template<typename T, typename Allocator>
void Vector<T, Allocator>::clear() {
    for (size_t i = 0; i < size_; ++i) {
        alloc_traits::destroy(alloc_, data_ + i);
    }
    size_ = 0;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::erase_swap(const_iterator pos) {
    size_t index = pos - data_;
    if (index >= size_) return end();

    alloc_traits::destroy(alloc_, data_ + index);

    if (index != size_ - 1) {
        alloc_traits::construct(alloc_, data_ + index, std::move(data_[size_ - 1]));
        alloc_traits::destroy(alloc_, data_ + size_ - 1);
    }

    --size_;
    return (index < size_) ? data_ + index : end();
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::remove_swap(const T &value) {
    for (size_t i = 0; i < size_; ++i) {
        if (data_[i] == value) {
            return erase_swap(data_ + i);
        }
    }
    return end();
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::erase(const_iterator pos) {
    size_t index = pos - data_;
    alloc_traits::destroy(alloc_, data_ + index);

    for (size_t i = index; i < size_ - 1; ++i) {
        alloc_traits::construct(alloc_, data_ + i, std::move(data_[i + 1]));
        alloc_traits::destroy(alloc_, data_ + (i + 1));
    }

    --size_;
    return data_ + index;
}

template<typename T, typename Allocator>
const T &Vector<T, Allocator>::back() const {
    if (size_ == 0) throw std::out_of_range("Vector is empty");
    return data_[size_ - 1];
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::back() {
    if (size_ == 0) throw std::out_of_range("Vector is empty");
    return data_[size_ - 1];
}

template<typename T, typename Allocator>
const T &Vector<T, Allocator>::front() const {
    if (size_ == 0) throw std::out_of_range("Vector is empty");
    return data_[0];
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::front() {
    if (size_ == 0) throw std::out_of_range("Vector is empty");
    return data_[0];
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::at(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("index out of range");
    }
    return data_[index];
}

template<typename T, typename Allocator>
const T &Vector<T, Allocator>::at(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("index out of range");
    }
    return data_[index];
}

template<typename T, typename Allocator>
const T &Vector<T, Allocator>::operator[](size_t index) const noexcept {
    return data_[index];
}

template<typename T, typename Allocator>
T &Vector<T, Allocator>::operator[](size_t index) noexcept {
    return data_[index];
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector()
        : alloc_(), data_(nullptr), size_(0), capacity_(0) {}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector &other) : alloc_(
        alloc_traits::select_on_container_copy_construction(other.alloc_)) {
    reserve(other.capacity_);
    for (size_t i = 0; i < other.size_; ++i) {
        alloc_traits::construct(alloc_, data_ + i, other.data_[i]);
    }
    size_ = other.size_;
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_t size, const T& value)
        : alloc_(), data_(nullptr) {
    if (size == 0) return;

    reserve(size);

    for (size_t i = 0; i < size; ++i) {
        alloc_traits::construct(alloc_, data_ + i, value);
    }

    size_ = size;
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_t size)
        : alloc_(), data_(nullptr) {
    if (size == 0) return;

    reserve(size);

    for (size_t i = 0; i < size; ++i) {
        alloc_traits::construct(alloc_, data_ + i);
    }

    size_ = size;
}


template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::begin() noexcept {
    return data_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::end() noexcept {
    return data_ + size_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::begin() const noexcept {
    return data_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::end() const noexcept {
    return data_ + size_;
}

template<typename T, typename Allocator>
size_t Vector<T, Allocator>::size() const noexcept {
    return size_;
}

template<typename T, typename Allocator>
size_t Vector<T, Allocator>::capacity() const noexcept {
    return capacity_;
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::empty() const noexcept {
    return size_ == 0;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::reserve(size_t new_capacity) noexcept {
    if (new_capacity <= capacity_) {
        return;
    }

    iterator new_data = alloc_traits::allocate(alloc_, new_capacity);

    for (size_t i = 0; i < size_; ++i) {
        try {
            alloc_traits::construct(alloc_, new_data + i, std::move(data_[i]));
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                alloc_traits::destroy(alloc_, new_data + j);
            }

            alloc_traits::deallocate(alloc_, new_data, new_capacity);
            throw;
        }
    }

    for (size_t i = 0; i < size_; ++i) {
        alloc_traits::destroy(alloc_, data_ + i);
    }

    if (data_) {
        alloc_traits::deallocate(alloc_, data_, capacity_);
    }

    data_ = new_data;
    capacity_ = new_capacity;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_t n) {
    if (n > capacity_) {
        reserve(n);
    }

    if (n > size_) {
        for (size_t i = size_; i < n; ++i) {
            alloc_traits::construct(alloc_, data_ + i);
        }
    } else {
        for (size_t i = n; i < size_; ++i) {
            alloc_traits::destroy(alloc_, data_ + i);
        }
    }

    size_ = n;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::push_back(const T &value) noexcept {
    if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    alloc_traits::construct(alloc_, data_ + size_, value);
    ++size_;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::push_back(T &&value) noexcept {
    if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    alloc_traits::construct(alloc_, data_ + size_, std::move(value));
    ++size_;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::pop_back() noexcept {
    if (size_ > 0) {
        --size_;
        alloc_traits::destroy(alloc_, data_ + size_);
    }
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::del(const T &value) {
    for (size_t i = 0; i < size_; ++i) {
        if (data_[i] == value) {
            return erase(data_ + i);
        }
    }
    return end();
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(const_iterator pos, const T &value) {
    size_t index = pos - data_;

    if (size_ >= capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }

    for (size_t i = size_; i > index; --i) {
        alloc_traits::construct(alloc_, data_ + i, std::move(data_[i - 1]));
        alloc_traits::destroy(alloc_, data_ + (i - 1));
    }

    alloc_traits::construct(alloc_, data_ + index, value);
    ++size_;
    return data_ + index;
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& other) {
    if (this == &other)
        return *this;

    clear();

    if (capacity_ < other.size_) {
        if (data_) {
            alloc_traits::deallocate(alloc_, data_, capacity_);
        }
        data_ = alloc_traits::allocate(alloc_, other.capacity_);
        capacity_ = other.capacity_;
    }

    for (size_t i = 0; i < other.size_; ++i) {
        alloc_traits::construct(alloc_, data_ + i, other.data_[i]);
    }

    size_ = other.size_;
    return *this;
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& other) noexcept {
    if (this == &other)
        return *this;

    clear();

    if (data_) {
        alloc_traits::deallocate(alloc_, data_, capacity_);
    }

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}

template<typename T, typename Allocator>
Vector<T, Allocator>::~Vector() {
    clear();
    if (data_) {
        alloc_traits::deallocate(alloc_, data_, capacity_);
    }
}


#endif //VECTOR_H
