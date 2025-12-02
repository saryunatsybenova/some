//
// Created by sphdx on 6/23/25.
//

#ifndef STUDENTGRADE_H
#define STUDENTGRADE_H

#include <sstream>
#include <string>

#include "Date.h"
#include "PersonName.h"
#include "Student.h"
#include "Grade.h"

namespace model {
    class StudentGrade {
        PersonName student_name_;
        std::string class_;
        Date birth_date_;

        std::string subject_;
        int grade_{};
        Date grade_date_;

    public:
        StudentGrade(PersonName student_name, std::string class_name, Date birth_date,
                     std::string subject, int grade, Date grade_date);

        StudentGrade(const Student &student, const Grade &grade);

        StudentGrade();

        bool operator<(const StudentGrade &other) const;
        bool operator>(const StudentGrade &other) const;
        bool operator==(const StudentGrade &other) const;
        bool operator!() const;

        [[nodiscard]] PersonName get_student_name() const;
        [[nodiscard]] std::string get_class() const;
        [[nodiscard]] Date get_birth_date() const;
        [[nodiscard]] std::string get_subject() const;
        [[nodiscard]] int get_grade() const;
        [[nodiscard]] Date get_grade_date() const;

        [[nodiscard]] std::string to_string() const;
        static StudentGrade parse(const std::string &line);

        friend std::ostream &operator<<(std::ostream &os, const StudentGrade &sg);
    };

    inline StudentGrade::StudentGrade(PersonName student_name, std::string class_name, Date birth_date,
                                      std::string subject, const int grade, Date grade_date)
        : student_name_(std::move(student_name)),
          class_(std::move(class_name)),
          birth_date_(birth_date),
          subject_(std::move(subject)),
          grade_(grade),
          grade_date_(grade_date) {
    }

    inline StudentGrade::StudentGrade(const Student &student, const Grade &grade_obj)
        : student_name_(student.get_name()),
          class_(std::to_string(student.get_class())),
          birth_date_(student.get_birth_date()),
          subject_(grade_obj.get_subject()),
          grade_(grade_obj.get_grade()),
          grade_date_(grade_obj.get_date()) {
    }

    inline StudentGrade::StudentGrade() = default;

    inline bool StudentGrade::operator<(const StudentGrade &other) const {
        if (grade_date_ != other.grade_date_) return grade_date_ < other.grade_date_;
        if (student_name_ != other.student_name_) return student_name_ < other.student_name_;
        if (class_ != other.class_) return class_ < other.class_;
        if (birth_date_ != other.birth_date_) return birth_date_ < other.birth_date_;
        if (subject_ != other.subject_) return subject_ < other.subject_;
        return grade_ < other.grade_;
    }

    inline bool StudentGrade::operator>(const StudentGrade &other) const {
        return other < *this;
    }

    inline bool StudentGrade::operator==(const StudentGrade &other) const {
        return grade_date_ == other.grade_date_ &&
               student_name_ == other.student_name_ &&
               class_ == other.class_ &&
               birth_date_ == other.birth_date_ &&
               subject_ == other.subject_ &&
               grade_ == other.grade_;
    }

    inline bool StudentGrade::operator!() const {
        return !student_name_ && class_.empty() && !birth_date_ &&
               subject_.empty() && grade_ == 0 && !grade_date_;
    }

    inline PersonName StudentGrade::get_student_name() const { return student_name_; }
    inline std::string StudentGrade::get_class() const { return class_; }
    inline Date StudentGrade::get_birth_date() const { return birth_date_; }
    inline std::string StudentGrade::get_subject() const { return subject_; }
    inline int StudentGrade::get_grade() const { return grade_; }
    inline Date StudentGrade::get_grade_date() const { return grade_date_; }

    inline std::string StudentGrade::to_string() const {
        std::ostringstream os;
        os << student_name_.to_string() << '\t' << class_ << '\t' << birth_date_.to_string() << '\t'
           << subject_ << '\t' << grade_ << '\t'
           << grade_date_.to_string();
        return os.str();
    }

    inline std::ostream &operator<<(std::ostream &os, const StudentGrade &sg) {
        return os << sg.to_string();
    }

    inline StudentGrade StudentGrade::parse(const std::string &line) {
        std::istringstream iss(line);

        std::string s_name, class_name, birth_date_str;
        std::string subject, grade_str, grade_date_str;

        auto parse_column = [&](std::string &out) {
            if (!std::getline(iss, out, '\t'))
                throw std::invalid_argument("Данные студента-оценки некорректны, слишком мало столбцов");
        };

        parse_column(s_name);
        parse_column(class_name);
        parse_column(birth_date_str);
        parse_column(subject);
        parse_column(grade_str);
        std::getline(iss, grade_date_str);

        if (iss.peek() != std::char_traits<char>::eof())
            throw std::invalid_argument("Данные студента-оценки некорректны, слишком много столбцов");

        int grade = 0;
        try {
            grade = std::stoi(grade_str);
        } catch (const std::exception &) {
            throw std::invalid_argument("Некорректная оценка");
        }

        return StudentGrade{
            PersonName::parse(s_name), class_name, Date::parse(birth_date_str),
            subject, grade,
            Date::parse(grade_date_str)
        };
    }
}

#endif //STUDENTGRADE_H