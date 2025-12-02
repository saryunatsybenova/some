//
// Created by sphdx on 6/22/25.
//

#ifndef STUDENTREPO_H
#define STUDENTREPO_H

#include <string>

#include "Repository.h"
#include "../utils/FileReader.h"
#include "../model/Student.h"
#include "../hash/HashTable.h"
#include "vector/Vector.h"

namespace repo {
    class StudentRepo {
        Vector<model::Student> students_{};

        hash::HashTable<std::string, size_t> table_;

        ToKey to_key_{};

    public:
        StudentRepo();
        ~StudentRepo();

        explicit StudentRepo(const std::string &file_path, ToKey to_key, size_t hash_table_cap = 0);

        bool add_student(const model::Student &student);
        bool del_student(const model::Student &student);
        const model::Student * search_student(const std::string &key, size_t &steps);

        [[nodiscard]] size_t size() const;
        [[nodiscard]] Vector<model::Student> students() const;

        [[nodiscard]] std::string table_structure(bool show_only_occupied) const;
    };

    inline StudentRepo::StudentRepo(const std::string &file_path, const ToKey to_key, const size_t hash_table_cap) {
        std::size_t count = 0;
        students_ = utils::FileReader::read_file<model::Student>(file_path, count);
        to_key_ = to_key;

        const size_t table_cap = hash_table_cap == 0 ? students_.size() * 2 : hash_table_cap;
        table_ = hash::HashTable<std::string, size_t>(table_cap);

        Slog::info("Статическая хеш-таблица инициализирована", Slog::opt("ёмкость", table_cap));

        for (std::size_t i = 0; i < students().size(); ++i) {
            
            table_.append(to_key_(students_[i].get_name(), students_[i].get_birth_date().to_string()), i);
            
        }

        Slog::info("Справочник учеников инициализирован");
    }

    inline StudentRepo::~StudentRepo() = default;

    inline bool StudentRepo::add_student(const model::Student& student) {
        const std::string key = to_key_(student.get_name(), student.get_birth_date().to_string());
        const auto new_size = students().size() + 1;
        try {
            table_.append(key, new_size - 1);
        } catch (std::overflow_error &e) {
            throw std::overflow_error(e.what());
            return false;
        }

        students_.push_back(student);

        return true;
    }

    inline bool StudentRepo::del_student(const model::Student &student) {
        const std::string key = to_key_(student.get_name(), student.get_birth_date().to_string());
        
        // находим ключ с помощью хеш-таблицы
        const auto entry = table_.search(key, []{});
        if (entry == nullptr)
            return false; // ключ не найден

        std::size_t idx = *entry->val();

        // проверяем полное совпадение объектов
        if (students_[idx] != student)
            return false; // объекты не идентичны

        // удаляем из ХТ
        table_.del(key, idx);

        // удаляем из массива
        students_.erase_swap(students_.begin() + idx);

        // обновляем индексы в хеш-таблице для всех элементов после удаленного
        for (std::size_t i = idx; i < students().size(); ++i) {
            table_.update(
                to_key_(students_[i].get_name(), students_[i].get_birth_date().to_string()),
                i, []{});
        }

        return true;
    }

    inline const model::Student * StudentRepo::search_student(const std::string &key, size_t &steps) {
        auto visit = [&steps]() {
            ++steps;
        };
        const auto node = table_.search(key, visit);
        if (node == nullptr) return nullptr;

        return &students_[*node->val()];
    }

    inline size_t StudentRepo::size() const {
        return students().size();
    }

    inline Vector<model::Student> StudentRepo::students() const {
        return students_;
    }

    inline std::string StudentRepo::table_structure(const bool show_only_occupied) const {
        return table_.structure(show_only_occupied);
    }
}

#endif //STUDENTREPO_H