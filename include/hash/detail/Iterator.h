//
// Created by sphdx on 5/17/25.
//

#ifndef ITERATOR_H
#define ITERATOR_H
#include <cstddef>
#include <iterator>

#include "Entry.h"

namespace hash::detail {
    template<typename Container>
    class Iterator {
        using EntryType = typename Container::EntryType;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = EntryType;
        using pointer = EntryType *;
        using reference = EntryType &;
        using iterator_category = std::forward_iterator_tag;

        Iterator(Container *tbl, const std::size_t idx)
            : tbl_(tbl), idx_(idx) {
            advance_to_next();
        }

        reference operator*() const { return tbl_->table_[idx_]; }
        pointer operator->() const { return &tbl_->table_[idx_]; }

        // pre‐increment
        Iterator &operator++() {
            ++idx_;
            advance_to_next();
            return *this;
        }

        // post‐increment
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(Iterator const &o) const {
            return tbl_ == o.tbl_ && idx_ == o.idx_;
        }

        bool operator!=(Iterator const &o) const {
            return !(*this == o);
        }

    private:
        Container *tbl_;
        std::size_t idx_;

        void advance_to_next() {
            while (idx_ < tbl_->cap_
                   && tbl_->table_[idx_].status() != OCCUPIED) {
                ++idx_;
            }
        }
    };
}

#endif //ITERATOR_H
