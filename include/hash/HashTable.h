//
// Created by sphdx on 5/18/25.
//

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <sstream>
#include "detail/Entry.h"
#include "detail/EntryStatus.h"
#include "detail/Iterator.h"
#include "../Slog.h"

namespace hash {
    template<typename Key, typename Val>
    class HashTable {
    public:
        using EntryType = detail::Entry<Key, Val>;
        using iterator = detail::Iterator<HashTable>;
        using const_iterator = detail::Iterator<HashTable const>;

    private:
        template<typename Container>
        friend class detail::Iterator;

        // table capacity
        size_t cap_;
        // count of elements in table
        size_t size_;

        EntryType *table_ = nullptr;

        // Простая хеш-функция для строк
        size_t primary_hash(const Key &key) const;

        // Вторичная хеш-функция
        [[nodiscard]] size_t secondary_hash(size_t primary, size_t attempt) const;

        size_t resolve_collision(size_t primary_hash, const Key &key);

        size_t prepare_append(const Key &key);

        template<typename Callback>
        size_t find(const Key &key, Callback &&visit) const;

        template<typename Callback>
        size_t find(const Key &key, const Val &val, Callback &&visit) const;

    public:
        explicit HashTable(size_t cap = 16);

        ~HashTable();

        void append(const Key &key, const Val &val);

        template<typename Callback>
        void update(const Key &key, const Val &val, Callback &&visit);

        const EntryType *del(const Key &key, const Val &val);

        const EntryType *del(const Key &key);

        template<typename Callback>
        const EntryType *search(const Key &key, Callback &&visit) const;

        template<typename Callback>
        const EntryType *search(const Key &key, const Val &val, Callback &&visit) const;

        HashTable(const HashTable &) = delete;

        HashTable &operator=(const HashTable &) = delete;

        HashTable &operator=(HashTable &&other) noexcept;

        HashTable(HashTable &&other) noexcept;

        iterator begin();

        iterator end();

        const_iterator begin() const;

        const_iterator end() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] size_t capacity() const;

        [[nodiscard]] bool empty() const;

        [[nodiscard]] std::string structure(bool show_only_occupied = true) const;
    };

    template<typename Key, typename Val>
    size_t HashTable<Key, Val>::primary_hash(const Key &key) const {
        if constexpr (std::is_same_v<Key, std::string>) {
            size_t sum = 0;
            for (const char c: key) {
                sum += static_cast<size_t>(c);
            }

            size_t hash = sum % cap_;

            Slog::info("Первичная хеш функция", Slog::opt("хеш", hash));

            return hash;
        }

        return std::hash<Key>{}(key) % cap_;
    }

    template<typename Key, typename Val>
    size_t HashTable<Key, Val>::secondary_hash(size_t primary, size_t attempt) const {
        size_t hash = (primary + attempt) % cap_;
        Slog::info("Вторичная хеш функция", Slog::opt("хеш", hash));
        return hash;
    }

    template<typename Key, typename Val>
    size_t HashTable<Key, Val>::resolve_collision(size_t primary_hash, const Key &key) {
        size_t attempt = 1, deleted_entry = 0, current_index = primary_hash;
        bool deleted_entry_found = false;

        while (attempt < cap_ && table_[current_index].status() != detail::EMPTY) {
            if (table_[current_index].status() == detail::DELETED && !deleted_entry_found) {
                deleted_entry = current_index;
                deleted_entry_found = true;
            }

            if (table_[current_index].status() == detail::OCCUPIED &&
                *table_[current_index].key() == key) {
                throw std::overflow_error(
                    "Разрешить коллизию не удалось, ключ уже существует: " + key
                );
            }

            current_index = secondary_hash(primary_hash, attempt);
            attempt++;
        }

        if (attempt == cap_) {
            if (!deleted_entry_found) {
                throw std::overflow_error(
                    "Разрешить коллизию не удалось, в таблице нет места"
                );
            }
            return deleted_entry;
        }
        if (deleted_entry_found) {
            return deleted_entry;
        }

        return current_index;
    }

    template<typename Key, typename Val>
    size_t HashTable<Key, Val>::prepare_append(const Key &key) {
        size_t p_hash = primary_hash(key);

        if (table_[p_hash].status() == detail::EMPTY) {
            return p_hash;
        }

        const size_t index = resolve_collision(p_hash, key);
        return index;
    }

    template<typename Key, typename Val>
    HashTable<Key, Val>::HashTable(const size_t cap) : cap_(cap > 3 ? cap : 16), size_(0) {
        table_ = new EntryType[cap_];
    }

    template<typename Key, typename Val>
    HashTable<Key, Val>::~HashTable() {
        delete[] table_;
    }

    template<typename Key, typename Val>
    void HashTable<Key, Val>::append(const Key &key, const Val &val) {
        if (size_ >= cap_) {
            throw std::overflow_error("Таблица заполнена");
        }

        size_t index = prepare_append(key);
        table_[index].insert(key, val);
        ++size_;
        Slog::info("Элемент добавлен",
                   Slog::opt("индекс", index),
                   Slog::opt("ключ", key),
                   Slog::opt("значение", val)
        );
    }

    template<typename Key, typename Val>
    template<typename Callback>
    size_t HashTable<Key, Val>::find(const Key &key, Callback &&visit) const {
        size_t primary_hash_index = primary_hash(key);
        visit();

        if (table_[primary_hash_index].status() == detail::OCCUPIED &&
            table_[primary_hash_index].key() != nullptr &&
            *table_[primary_hash_index].key() == key) {
            Slog::info("Поиск остановлен, ключ найден после вызова первичной хеш-функции",
                       Slog::opt("ключ", key),
                       Slog::opt("индекс", primary_hash_index)
            );
            return primary_hash_index;
        }

        size_t attempt = 1;
        size_t current_index = secondary_hash(primary_hash_index, attempt);

        while (attempt < cap_) {
            visit();
            const auto &entry = table_[current_index];

            if (entry.status() == detail::EMPTY) {
                Slog::info("Найдена пустая ячейка, поиск остановлен");
                return cap_;
            }

            if (entry.status() == detail::OCCUPIED &&
                entry.key() != nullptr &&
                *entry.key() == key) {
                break;
            }

            ++attempt;
            current_index = secondary_hash(primary_hash_index, attempt);
        }

        if (attempt == cap_) {
            return cap_;
        }

        Slog::info("Поиск остановлен, ключ найден",
                   Slog::opt("ключ", key),
                   Slog::opt("индекс", current_index)
        );

        return current_index;
    }

    template<typename Key, typename Val>
    template<typename Callback>
    size_t HashTable<Key, Val>::find(const Key &key, const Val &val, Callback &&visit) const {
        size_t primary_hash_index = primary_hash(key);
        visit();

        if (table_[primary_hash_index].status() == detail::OCCUPIED &&
            table_[primary_hash_index].key() != nullptr &&
            table_[primary_hash_index].val() != nullptr &&
            *table_[primary_hash_index].key() == key &&
            *table_[primary_hash_index].val() == val) {
            Slog::info("Поиск остановлен, ключ найден после вызова первичной хеш-функции",
                       Slog::opt("ключ", key),
                       Slog::opt("индекс", primary_hash_index)
            );
            return primary_hash_index;
        }

        size_t attempt = 0;
        size_t current_index = primary_hash_index;

        while (attempt < cap_) {
            visit();

            const auto &entry = table_[current_index];

            if (entry.status() == detail::EMPTY) {
                // не найдено
                return cap_;
            }

            if (entry.status() == detail::OCCUPIED &&
                entry.key() != nullptr &&
                entry.val() != nullptr &&
                *entry.key() == key &&
                *entry.val() == val) {
                Slog::info("Поиск остановлен, ключ найден",
                           Slog::opt("ключ", key),
                           Slog::opt("индекс", current_index));
                return current_index;
            }

            ++attempt;
            current_index = secondary_hash(primary_hash_index, attempt);
        }

        return cap_;
    }

    template<typename Key, typename Val>
    template<typename Callback>
    const typename HashTable<Key, Val>::EntryType *HashTable<Key, Val>::search(
        const Key &key, Callback &&visit) const {
        if (size_ < 1) return nullptr;
        auto index = this->find(key, std::forward<Callback>(visit));
        if (index == cap_) return nullptr;
        return &table_[index];
    }

    template<typename Key, typename Val>
    template<typename Callback>
    const typename HashTable<Key, Val>::EntryType *HashTable<Key, Val>::search(
        const Key &key, const Val &val, Callback &&visit) const {
        auto index = this->find(key, val, std::forward<Callback>(visit));
        if (index == cap_) return nullptr;
        return &table_[index];
    }

    template<typename Key, typename Val>
    template<typename Callback>
    void HashTable<Key, Val>::update(const Key &key, const Val &val, Callback &&visit) {
        auto index = this->find(key, std::forward<Callback>(visit));
        if (index == cap_) return;
        auto &entry = table_[index];
        entry.insert(key, val);
        Slog::info("Элемент обновлен",
                   Slog::opt("ключ", key),
                   Slog::opt("значение", val)
        );
    }

    template<typename Key, typename Val>
    const typename HashTable<Key, Val>::EntryType *HashTable<Key, Val>::del(
        const Key &key, const Val &val) {
        auto index = this->find(key, val, [] {
        });
        if (index == cap_) return nullptr;
        auto *el = &table_[index];
        el->del();
        if (size_ > 0) --size_;
        return el;
    }

    template<typename Key, typename Val>
    const typename HashTable<Key, Val>::EntryType *HashTable<Key, Val>::del(const Key &key) {
        auto index = this->find(key, [] {
        });
        if (index == cap_) return nullptr;
        auto *el = &table_[index];
        el->del();
        if (size_ > 0) --size_;
        return el;
    }

    template<typename Key, typename Val>
    size_t HashTable<Key, Val>::size() const {
        return size_;
    }

    template<typename Key, typename Val>
    size_t HashTable<Key, Val>::capacity() const {
        return cap_;
    }

    template<typename Key, typename Val>
    bool HashTable<Key, Val>::empty() const {
        return size_ == 0;
    }

    template<typename Key, typename Val>
    std::string HashTable<Key, Val>::structure(bool show_only_occupied) const {
        std::ostringstream oss;

        for (size_t i = 0; i < cap_; ++i) {
            std::ostringstream key_stream, val_stream;

            if (show_only_occupied) {
                if (table_[i].status() == detail::OCCUPIED) {
                    key_stream << *table_[i].key();
                    val_stream << *table_[i].val();
                }
            } else {
                if (table_[i].status() == detail::EMPTY) {
                    key_stream << "empty";
                    val_stream << "empty";
                } else if (table_[i].status() == detail::DELETED) {
                    key_stream << "deleted";
                    val_stream << "deleted";
                } else {
                    key_stream << *table_[i].key();
                    val_stream << *table_[i].val();
                }
            }

            std::string key_str = key_stream.str();
            std::string val_str = val_stream.str();

            if (!key_str.empty() && !val_str.empty()) {
                oss << i << " " << key_str
                        << " - " << val_str
                        << "\n";
            }
        }

        return oss.str();
    }

    template<typename Key, typename Val>
    HashTable<Key, Val> &HashTable<Key, Val>::operator=(HashTable &&other) noexcept {
        if (this != &other) {
            delete[] table_;

            cap_ = other.cap_;
            size_ = other.size_;
            table_ = other.table_;

            other.table_ = nullptr;
            other.cap_ = 0;
            other.size_ = 0;
        }
        return *this;
    }

    template<typename Key, typename Val>
    HashTable<Key, Val>::HashTable(HashTable &&other) noexcept
        : cap_(other.cap_), size_(other.size_), table_(other.table_) {
        other.table_ = nullptr;
        other.cap_ = 0;
        other.size_ = 0;
    }

    template<typename Key, typename Val>
    typename HashTable<Key, Val>::iterator HashTable<Key, Val>::begin() {
        return iterator(this, 0);
    }

    template<typename Key, typename Val>
    typename HashTable<Key, Val>::iterator HashTable<Key, Val>::end() {
        return iterator(this, cap_);
    }

    template<typename Key, typename Val>
    typename HashTable<Key, Val>::const_iterator HashTable<Key, Val>::begin() const {
        return const_iterator(this, 0);
    }

    template<typename Key, typename Val>
    typename HashTable<Key, Val>::const_iterator HashTable<Key, Val>::end() const {
        return const_iterator(this, cap_);
    }
}

#endif //HASHTABLE_H
