//
// Created by sphdx on 6/23/25.
//

#ifndef SCHOOLREPO_H
#define SCHOOLREPO_H

#include <string>

#include "GradeRepo.h"
#include "StudentRepo.h"
#include "Repository.h"
#include "../model/Student.h"
#include "../model/StudentGrade.h"
#include "../utils/FileWriter.h"

namespace repo {
    class SchoolRepo {
        StudentRepo student_repo_;
        GradeRepo grade_repo_;

        ToKey to_key_{};

    public:
        SchoolRepo() = delete;
        ~SchoolRepo() = default;

        explicit SchoolRepo(
            const std::string &student_dir_path,
            const std::string &grade_dir_path,
            ToKey to_key,
            size_t hash_table_cap = 0
        );

        bool add_student(const model::Student &student);
        bool del_student(const model::Student &student);
        const model::Student *search_student(const std::string &key, size_t &steps);

        bool add_grade(model::Grade &grade);
        bool del_grade(const model::Grade &grade);
        size_t del_grades(const std::string &key);
        Vector<model::Grade> search_grades(const std::string &key, size_t &steps) const;

        Vector<model::StudentGrade> get_filtered(const model::Date &student_birth_date, const std::string &subject,
                                               model::Date start_period, model::Date end_period, size_t &steps);

        void save_student_repo(const std::string& path) const;
        void save_grade_repo(const std::string& path) const;

        static void save_filtered(const std::string &path, const Vector<model::StudentGrade> &student_grades, size_t count);

        [[nodiscard]] size_t student_repo_size() const;
        [[nodiscard]] Vector<model::Student> students() const;

        [[nodiscard]] size_t grade_repo_size() const;
        [[nodiscard]] Vector<model::Grade> grades() const;

        [[nodiscard]] std::string key_tree_structure(bool horizontal = false) const;
        [[nodiscard]] std::string date_tree_structure(bool horizontal = false) const;
        [[nodiscard]] std::string table_structure(bool show_only_occupied = true) const;
    };

    inline SchoolRepo::SchoolRepo(
        const std::string &student_dir_path,
        const std::string &grade_dir_path,
        const ToKey to_key,
        const size_t hash_table_cap
    ) : student_repo_(StudentRepo(student_dir_path, to_key, hash_table_cap)),
        grade_repo_(GradeRepo(grade_dir_path, to_key)),
        to_key_(to_key) {
        // проверяем целостность записей
        size_t count = 0;
        const auto *keys = grade_repo_.keys(count);

        Slog::info("Проверка целостности данных");

        size_t deleted = 0;

        // затычка (вместо подсчета шагов)
        size_t tmp = 0;
        // проходимся по ключам
        // если ключа нет - выкидываем ошибку (целостность нарушена)
        for (size_t i = 0; i < count; ++i) {
            //search_student возвращает Student
            if (student_repo_.search_student(keys[i], tmp) == nullptr) {
                Slog::info("Целостность данных нарушена",
                    Slog::opt("ключ", keys[i]));
                // все-таки ключа нет, целостность нарушена, выкидываем ошибку
                throw std::runtime_error("Оценка отсылает на отсутствующего студента");
            }
        }

        Slog::info("Проверка целостности данных завершена",
            Slog::opt("удалено_ключей", deleted));
    }

    inline bool SchoolRepo::add_student(const model::Student &student) {
        return student_repo_.add_student(student);
    }

    inline bool SchoolRepo::del_student(const model::Student &student) {
        // формируем ключ для проверки связанных записей
        const std::string key = to_key_(student.get_name(), student.get_birth_date().to_string());
        
        if (size_t tmp = 0; !grade_repo_.search_grades(key, tmp).empty()) {
            // нашли записи в справочнике оценок - выбрасываем ошибку
            throw std::invalid_argument("Для переданного студента найдена запись(-и) в таблице оценок. "
                                     "Сначала удалите связанные оценки.");
        }

        // связанных записей нет - удаляем запись о студенте
        //del_student возвращает bool
        const auto deleted = student_repo_.del_student(student);

        Slog::info("Удаление студента",
            Slog::opt("ключ", key),
            Slog::opt("успех", deleted ? "да" : "нет"));

        return deleted;
    }

    inline const model::Student *SchoolRepo::search_student(const std::string &key, size_t &steps) {
        Slog::info("Поиск студента",
            Slog::opt("ключ", key));

        auto *student = student_repo_.search_student(key, steps);

        Slog::info("Поиск студента завершился",
            Slog::opt("найдено", student == nullptr ? "нет" : "да"));

        return student;
    }

    inline bool SchoolRepo::add_grade(model::Grade &grade) {
        Slog::info("Добавление оценки",
            Slog::opt("данные", grade));

        // проверяем наличие студента
        // если есть - добавляем, иначе отдаем ошибку
        size_t tmp = 0;
        auto *student = student_repo_.search_student(
            to_key_(grade.get_student_name(), grade.get_student_birth_date().to_string()), tmp
        );
        if (student == nullptr) {
            throw std::invalid_argument("Студента не существует");
            return false;
        }

        const auto deleted = grade_repo_.add_grade(grade);

        Slog::info("Добавление завершено",
            Slog::opt("данные", grade),
            Slog::opt("успешно", deleted ? "да" : "нет"));

        return deleted;
    }

    inline bool SchoolRepo::del_grade(const model::Grade &grade) {
        Slog::info("Удаление оценки",
            Slog::opt("данные", grade));

        const auto deleted = grade_repo_.del_grade(grade);

        Slog::info("Удаление завершено",
            Slog::opt("успешно", deleted == true ? "да" : "нет"));

        return deleted;
    }

    inline Vector<model::Grade> SchoolRepo::search_grades(const std::string &key, size_t &steps) const {
        Slog::info("Поиск оценок",
            Slog::opt("ключ", key));

        auto grades = grade_repo_.search_grades(key, steps);

        Slog::info("Поиск закончен",
            Slog::opt("ключ", key),
            Slog::opt("количество", grades.size()));

        return grades;
    }

    inline Vector<model::StudentGrade> SchoolRepo::get_filtered(
        const model::Date &student_birth_date,
        const std::string &subject,
        const model::Date start_period, const model::Date end_period,
        size_t &steps
    ) {
        if (subject.empty())
            throw std::invalid_argument("Предмет не указан");

        if (start_period > end_period)
            throw std::invalid_argument("Период задан с ошибкой");

        size_t count = 0;

        // сначала ищем оценки по дате
        const auto grades = grade_repo_.search_in_date_range(start_period, end_period, steps);

        if (grades.empty())
            return {};

        Vector<model::StudentGrade> result;

        for (const auto & gr : grades) {
            // фильтрация по дате рождения
            if (gr.get_student_birth_date() != student_birth_date)
                continue;

            // фильтрация по предмету
            if (gr.get_subject() != subject)
                continue;

            // поиск студента по ключу (ФИО + дата рождения)
            std::string key = to_key_(gr.get_student_name(), gr.get_student_birth_date().to_string());
            const model::Student* student = student_repo_.search_student(key, steps);
            if (student == nullptr)
                continue;

            result.push_back(model::StudentGrade(*student, gr));
            ++count;
        }


        if (count == 0) {
            return {};
        }


        Slog::info("Справочник успешно сформирован",
            Slog::opt("дата_рождения", student_birth_date),
            Slog::opt("предмет", subject),
            Slog::opt("начало_периода", start_period),
            Slog::opt("конец_периода", end_period)
        );

        return result;
    }

    inline void SchoolRepo::save_student_repo(const std::string &path) const {
        utils::FileWriter::write_array(path, student_repo_.students(), student_repo_.size());
    }

    inline void SchoolRepo::save_grade_repo(const std::string &path) const {
        utils::FileWriter::write_array(path, grade_repo_.grades(), grade_repo_.size());
    }

    inline void SchoolRepo::save_filtered(const std::string &path, const Vector<model::StudentGrade> &student_grades, size_t count) {
        utils::FileWriter::write_array(path, student_grades, count);
    }

    inline size_t SchoolRepo::student_repo_size() const {
        return student_repo_.size();
    }

    inline Vector<model::Student> SchoolRepo::students() const {
        if (student_repo_.size() == 0)
            return {};

        return Vector(student_repo_.students());
    }

    inline size_t SchoolRepo::grade_repo_size() const {
        return grade_repo_.size();
    }

    inline Vector<model::Grade> SchoolRepo::grades() const {
        return Vector(grade_repo_.grades());
    }

    inline std::string SchoolRepo::key_tree_structure(const bool horizontal) const {
        return grade_repo_.key_tree_structure(horizontal);
    }

    inline std::string SchoolRepo::date_tree_structure(const bool horizontal) const {
        return grade_repo_.date_tree_structure(horizontal);
    }

    inline std::string SchoolRepo::table_structure(const bool show_only_occupied) const {
        return student_repo_.table_structure(show_only_occupied);
    }
}


#endif //SCHOOLREPO_H