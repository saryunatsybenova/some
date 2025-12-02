//
// Created by sphdx on 6/22/25.
//

#ifndef GRADE_H
#define GRADE_H
#include <sstream>
#include <string>

#include "PersonName.h"
#include "Date.h"

namespace model {
    class Grade {
        PersonName student_name_;
        Date student_birth_date_;
        std::string subject_;
        int grade_{};
        Date date_;
        size_t id{};

    public:
        Grade(PersonName student_name, Date student_birth_date, std::string subject, int grade, Date date);
        Grade(const Grade& other);
        Grade();

        bool operator<(const Grade &other) const;
        bool operator>(const Grade &other) const;
        bool operator==(const Grade &other) const;
        bool operator!() const;

        Grade& operator=(const Grade& other);

        [[nodiscard]] model::PersonName get_student_name() const;
        [[nodiscard]] Date get_student_birth_date() const;
        [[nodiscard]] std::string get_subject() const;
        [[nodiscard]] int get_grade() const;
        [[nodiscard]] Date get_date() const;

        friend std::ostream &operator<<(std::ostream &os, const Grade &grade);

        [[nodiscard]] std::string to_string() const;
        static Grade parse(const std::string &grade);

        [[nodiscard]] size_t get_id() const;
        void set_id(size_t id);
    };

    inline std::ostream &operator<<(std::ostream &os, const Grade &grade) {
        return os << grade.to_string();
    }

    inline bool Grade::operator<(const Grade &other) const {
        if (student_name_ != other.student_name_) return student_name_ < other.student_name_;
        if (student_birth_date_ != other.student_birth_date_) return student_birth_date_ < other.student_birth_date_;
        if (subject_ != other.subject_) return subject_ < other.subject_;
        if (grade_ != other.grade_) return grade_ < other.grade_;
        return date_ < other.date_;
    }

    inline bool Grade::operator>(const Grade &other) const {
        return other < *this;
    }

    inline bool Grade::operator==(const Grade &other) const {
        return student_name_ == other.student_name_ && student_birth_date_ == other.student_birth_date_ &&
               subject_ == other.subject_ && grade_ == other.grade_ && date_ == other.date_;
    }

    inline bool Grade::operator!() const {
        return !student_name_ && !student_birth_date_ && subject_.empty() && grade_ == 0 && !date_;
    }

    inline Grade & Grade::operator=(const Grade &other) {
        if (this != &other) {
            student_name_ = other.student_name_;
            student_birth_date_ = other.student_birth_date_;
            subject_ = other.subject_;
            grade_ = other.grade_;
            date_ = other.date_;
        }
        return *this;
    }

    inline model::PersonName Grade::get_student_name() const {
        return student_name_;
    }

    inline std::string Grade::get_subject() const {
        return subject_;
    }

    inline int Grade::get_grade() const {
        return grade_;
    }

    inline Date Grade::get_student_birth_date() const {
        return student_birth_date_;
    }

    inline Date Grade::get_date() const {
        return date_;
    }

    inline std::string Grade::to_string() const {
        return student_name_.to_string() + '\t' + student_birth_date_.to_string() + '\t' +
               subject_ + '\t' + std::to_string(grade_) + '\t' + date_.to_string();
    }

    inline Grade Grade::parse(const std::string &grade) {
        std::istringstream iss(grade);

        std::string name, birth_date_str, subject, grade_str, date_str;

        auto parse_column = [&](std::string &out) {
            if (!std::getline(iss, out, '\t'))
                throw std::invalid_argument("Некорректные данные об оценке, слишком мало столбцов");
        };

        parse_column(name);
        parse_column(birth_date_str);
        parse_column(subject);
        parse_column(grade_str);
        std::getline(iss, date_str);

        if (iss.peek() != std::char_traits<char>::eof())
            throw std::invalid_argument("Некорректные данные об оценке, слишком много столбцов");

        int grade_value = 0;
        try {
            grade_value = std::stoi(grade_str);
        } catch (const std::exception &) {
            throw std::invalid_argument("Некорректная оценка");
        }

        return Grade{
            model::PersonName::parse(name),
            Date::parse(birth_date_str),
            subject,
            grade_value,
            Date::parse(date_str)
        };
    }

    inline size_t Grade::get_id() const {
        return id;
    }

    inline void Grade::set_id(const size_t id) {
        this->id = id;
    }

    inline Grade::Grade(model::PersonName student_name, Date student_birth_date, std::string subject, const int grade, Date date) :
        student_name_(std::move(student_name)),
        student_birth_date_(student_birth_date),
        subject_(std::move(subject)),
        grade_(grade),
        date_(date) {
        if (subject_.empty())
            throw std::invalid_argument("Предмет не указан");
        if (grade_ < 1 || grade_ > 5)
            throw std::invalid_argument("Оценка должна быть от 1 до 5");
        if (!student_birth_date_)
            throw std::invalid_argument("Дата рождения студента не указана");
        if (!date)
            throw std::invalid_argument("Дата не указана");
    }

    inline Grade::Grade(const Grade &other) = default;

    inline Grade::Grade() = default;
}

#endif //GRADE_H