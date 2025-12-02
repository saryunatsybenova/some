//
// Created by sphdx on 6/23/25.
//

#ifndef GRADEREPO_H
#define GRADEREPO_H

#include <string>

#include "Repository.h"
#include "../utils/FileReader.h"
#include "../avl-tree/AVLTree.h"
#include "../model/Grade.h"
#include "list/List.h"

namespace repo {
    class GradeRepo {
        Vector<model::Grade> grades_{};

        AVLTree<std::string> key_tree_;
        AVLTree<model::Date> date_tree_;

        ToKey to_key_{};

    public:
        GradeRepo();
        ~GradeRepo();

        explicit GradeRepo(const std::string &file_path, ToKey to_key);

        bool add_grade(model::Grade &grade);
        bool del_grade(const model::Grade &grade);
        Vector<model::Grade> search_grades(const std::string &key, size_t &steps) const;

        [[nodiscard]] size_t size() const;
        [[nodiscard]] std::string * keys(size_t& count) const;
        [[nodiscard]] Vector<model::Grade> grades() const;

        [[nodiscard]] std::string key_tree_structure(bool horizontal) const;
        [[nodiscard]] std::string date_tree_structure(bool horizontal) const;

        Vector<model::Grade> search_in_date_range(model::Date low, model::Date high, size_t &steps) const;
    };

    inline GradeRepo::GradeRepo(const std::string &file_path, const ToKey to_key) {
        std::size_t count = 0;
        // загружаем оценки в массив
        grades_ = utils::FileReader::read_file<model::Grade>(file_path, count);
        // прокидываем функцию, которая превращает ФИО + дата рождения (как строка) в ключ
        to_key_ = to_key;

        // дерево ключей
        key_tree_ = AVLTree<std::string>();
        // дерево дат для фильтра
        date_tree_ = AVLTree<model::Date>();

        Vector<model::Grade> unique_grades;
        
        for (std::size_t i = 0; i < grades_.size(); ++i) {
            const auto key = to_key_(grades_[i].get_student_name(), grades_[i].get_student_birth_date().to_string());
            
            std::size_t ids_cnt = 0;
            const auto ids = key_tree_.to_arr(key, ids_cnt, []{});
            if (ids != nullptr) {
                for (size_t j = 0; j < ids_cnt; ++j) {
                    if (unique_grades[ids[j]] == grades_[i]) {
                        throw std::invalid_argument("Найден дубликат оценки");
                    }
                }
            }
            
            const auto new_index = unique_grades.size();
            unique_grades.push_back(grades_[i]);
            
            key_tree_.insert(key, static_cast<int>(new_index), []{});
            date_tree_.insert(grades_[i].get_date(), static_cast<int>(new_index));
        }
        
        grades_ = unique_grades;

        Slog::info("Справочник оценок инициализирован");
    }

    inline GradeRepo::~GradeRepo() = default;

    inline bool GradeRepo::add_grade(model::Grade& grade) {
        // такая оценка уже существует? да - выкидываем ошибку
        // обращаемся по ключу к дереву и получаем все id из списка в массив
        std::size_t ids_cnt = 0;
        const auto ids = key_tree_.to_arr(to_key_(grade.get_student_name(),
                grade.get_student_birth_date().to_string()), ids_cnt, []{});
        if (ids != nullptr) {
            for (size_t i = 0; i < ids_cnt; ++i) {
                if (grades_[ids[i]] == grade) {
                    return false;
                }
            }
        }

        // обновляем для оценки id (оценка будет добавлена в конец массива)
        const auto new_size = grades_.size() + 1;
        grade.set_id(grades_.size());

        // добавляем оценку в конец массива
        grades_.push_back(grade);

        // обновляем деревья, добавляем новый элемент
        key_tree_.insert(
            to_key_(grade.get_student_name(),
                grade.get_student_birth_date().to_string()),
                new_size - 1, []{});
        date_tree_.insert(grade.get_date(), new_size - 1, []{});

        Slog::info("Оценка добавлена", Slog::opt("данные", grade));

        return true;
    }

    inline bool GradeRepo::del_grade(const model::Grade &grade) {
        // оценки есть? нет - выходим
        if (grades_.empty())
            return false;

        const auto key = to_key_(grade.get_student_name(),
                                 grade.get_student_birth_date().to_string());

        // обращаемся по ключу к дереву и получаем все id из списка в массив
        std::size_t ids_cnt = 0;
        const auto ids = key_tree_.to_arr(key, ids_cnt, []{});

        // ничего не найдено
        if (ids_cnt == 0 || ids == nullptr)
            throw std::invalid_argument(
                "Список в узле дерева пуст");

        // находим первый попавшийся элемент, совпадающий с переданным
        std::size_t idx = grades_.size();
        for (std::size_t k = 0; k < ids_cnt; ++k)
            if (grades_[ids[k]] == grade) {
                idx = ids[k];
                break;
            }

        // ничего не нашли - ничего не удаляем
        if (idx == grades_.size())
            return false;

        // нашли оценку, удаляем ее из деревьев
        const auto& gr = grades_[idx];
        date_tree_.del(gr.get_date(), idx);
        key_tree_.del(key, idx);

        // если элемент не конечный, значит вместо него встанет последний элемент
        if (idx != grades_.size() - 1) {
            // последний элемент обновляем в деревьях
            auto last_grade = grades_.back();
            last_grade.set_id(idx);
            date_tree_.replace(last_grade.get_date(), grades_.size() - 1, idx);
            key_tree_.replace(to_key_(
                last_grade.get_student_name(),
                last_grade.get_student_birth_date().to_string()
                ),
                grades_.size() - 1, idx
            );
        }

        // удаляем оценку из массива, на её место ставим последний элемент
        grades_.erase_swap(grades_.begin() + idx);

        return true;
    }

    // search_grades выполняет поиск оценок соответствующих переданному ключу
    // счетчик steps отображает количество шагов поиска в дереве ключей
    inline Vector<model::Grade>
    GradeRepo::search_grades(const std::string &key, size_t &steps) const {
        // callback захватывает счетчик и увеличивает его при каждом рекурсивном вызове
        // в дереве
        const auto visit = [&steps] {
            ++steps;
        };

        // ищем узел совпадающий с переданным ключом
        const auto node = key_tree_.search(key, visit);
        // узел не найден - возвращаем пустой массив
        if (node == nullptr) return {};

        // узел нашелся, берем из него id (по которым лежат оценки)
        size_t count = node->list.count();
        Vector<model::Grade> grades;
        for (size_t i = 0; i < count; ++i) {
            // заполняем результат
            grades.push_back(grades_[node->list[i]]);
        }

        return grades;
    }

    inline size_t GradeRepo::size() const {
        return grades_.size();
    }

    inline std::string * GradeRepo::keys(size_t &count) const {
        count = key_tree_.get_nodes_count();
        return key_tree_.keys_in_order();
    }

    inline Vector<model::Grade> GradeRepo::grades() const {
        return grades_;
    }

    inline std::string GradeRepo::key_tree_structure(const bool horizontal) const {
        return horizontal ? key_tree_.structure() : key_tree_.lying_tree();
    }

    inline std::string GradeRepo::date_tree_structure(const bool horizontal) const {
        return horizontal ? date_tree_.structure() : date_tree_.lying_tree();
    }

    // search_in_date_range выполняет поиск оценок в рамках заданного периода
    // счетчик steps отображает количество шагов поиска в дереве дат
    inline Vector<model::Grade> GradeRepo::search_in_date_range(
        model::Date low, model::Date high, size_t &steps) const {
        size_t counter = 0;
        Vector indexes(grades_.size(), -1);

        date_tree_.range_search(low, high, [&](const List<int>& list) {
            ++steps;
            for (size_t i = 0; i < list.count(); ++i) {
                indexes[counter++] = list[i];
            }
        });

        if (counter == 0) {
            return {};
        }

        Vector<model::Grade> result(counter);
        for (size_t i = 0; i < counter; ++i) {
            result[i] = grades_[indexes[i]];
        }

        return result;
    }
}

#endif //GRADEREPO_H