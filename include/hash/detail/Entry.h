//
// Created by sphdx on 5/17/25.
//

#ifndef ENTRY_H
#define ENTRY_H
#include "EntryStatus.h"
#include "Pair.h"

namespace hash::detail {
    /**
    * @brief Ячейка хеш-таблицы
    *
    * Вызов конструктора без параметров создает ячейку без пары ключ-значение
    * со статусом EMPTY.
    * При вызове конструктора с параметром ключ-значение создает ячейку со
    * статусом OCCUPIED.
    * Метод del() меняет статус ячейки на DELETED.
    *
    */
    template<typename Key, typename Val>
    class Entry {
        Pair<Key, Val> *pair_;
        EntryStatus status_;

    public:
        Entry();
        Entry(const Key &key, const Val &val);

        bool operator==(const Entry &other) const;
        bool operator!=(const Entry &other) const;

        void insert(const Key &key, const Val &val);
        void del();

        [[nodiscard]] EntryStatus status() const;
        [[nodiscard]] Key *key() const;
        [[nodiscard]] Val *val() const;

        friend std::ostream& operator<<(std::ostream& os, Entry const& e) {
            os << "'" << *e.pair_->key() << "'" << "=" << "'" << *e.pair_->val() << "'";
            return os;
        }
    };

    template<typename Key, typename Val>
    Entry<Key, Val>::Entry() {
        pair_ = nullptr;
        status_ = EMPTY;
    }

    template<typename Key, typename Val>
    Entry<Key, Val>::Entry(const Key &key, const Val &val) {
        pair_ = new Pair<Key, Val>(key, val);
        status_ = OCCUPIED;
    }

    template<typename Key, typename Val>
    bool Entry<Key, Val>::operator==(const Entry &other) const {
        return status() == other.status() && *key() == *other.key() && *val() == *other.val();
    }

    template<typename Key, typename Val>
    bool Entry<Key, Val>::operator!=(const Entry &other) const {
        return !this->operator==(other);
    }

    template<typename Key, typename Val>
    void Entry<Key, Val>::insert(const Key &key, const Val &val) {
        delete pair_;
        pair_ = new Pair<Key, Val>(key, val);
        status_ = OCCUPIED;
    }

    template<typename Key, typename Val>
    void Entry<Key, Val>::del() {
        status_ = DELETED;
    }

    template<typename Key, typename Val>
    EntryStatus Entry<Key, Val>::status() const {
        return status_;
    }

    template<typename Key, typename Val>
    Key *Entry<Key, Val>::key() const {
        return pair_->key();
    }

    template<typename Key, typename Val>
    Val *Entry<Key, Val>::val() const {
        return pair_->val();
    }
}

#endif //ENTRY_H
