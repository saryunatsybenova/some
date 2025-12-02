//
// Created by sphdx on 6/22/25.
//

#ifndef STUDENT_H
#define STUDENT_H
#include <sstream>
#include <string>

#include "PersonName.h"
#include "Date.h"

namespace model {
    class Student {
        PersonName name_;
        int class_;
        Date birth_date_;
        size_t id{};

    public:
        Student(PersonName name, int class_number, Date birth_date);
        Student(const Student& other);
        Student();

        bool operator<(const Student &other) const;
        bool operator>(const Student &other) const;
        bool operator==(const Student &other) const;
        bool operator!() const;

        Student& operator=(const Student& other);

        [[nodiscard]] model::PersonName get_name() const;
        [[nodiscard]] int get_class() const;
        [[nodiscard]] Date get_birth_date() const;

        friend std::ostream &operator<<(std::ostream &os, const Student &student);

        [[nodiscard]] std::string to_string() const;
        static Student parse(const std::string &student);

        [[nodiscard]] size_t get_id() const;
        void set_id(size_t id);
    };

    inline std::ostream &operator<<(std::ostream &os, const Student &student) {
        return os << student.to_string();
    }

    inline bool Student::operator<(const Student &other) const {
        if (name_ != other.name_) return name_ < other.name_;
        if (class_ != other.class_) return class_ < other.class_;
        return birth_date_ < other.birth_date_;
    }

    inline bool Student::operator>(const Student &other) const {
        return other < *this;
    }

    inline bool Student::operator==(const Student &other) const {
        return name_ == other.name_ && class_ == other.class_ && birth_date_ == other.birth_date_;
    }

    inline bool Student::operator!() const {
        return !name_ && class_ == 0 && !birth_date_;
    }

    inline Student & Student::operator=(const Student &other) {
        if (this != &other) {
            name_ = other.name_;
            class_ = other.class_;
            birth_date_ = other.birth_date_;
        }
        return *this;
    }

    inline model::PersonName Student::get_name() const {
        return name_;
    }

    inline int Student::get_class() const {
        return class_;
    }

    inline Date Student::get_birth_date() const {
        return birth_date_;
    }

    inline std::string Student::to_string() const {
        return name_.to_string() + '\t' + std::to_string(class_) + '\t' + birth_date_.to_string();
    }

    inline Student Student::parse(const std::string &student) {
        std::istringstream iss(student);

        std::string name, class_str, birth_date_str;

        auto parse_column = [&](std::string &out) {
            if (!std::getline(iss, out, '\t'))
                throw std::invalid_argument("Некорректные данные о студенте, слишком мало столбцов");
        };

        parse_column(name);
        parse_column(class_str);
        std::getline(iss, birth_date_str);

        if (iss.peek() != std::char_traits<char>::eof())
            throw std::invalid_argument("Некорректные данные о студенте, слишком много столбцов");

        int class_number;
        try {
            class_number = std::stoi(class_str);
        } catch (const std::exception&) {
            throw std::invalid_argument("Класс должен быть числом от 1 до 11");
        }

        return Student{
            model::PersonName::parse(name),
            class_number,
            Date::parse(birth_date_str)
        };
    }

    inline size_t Student::get_id() const {
        return id;
    }

    inline void Student::set_id(const size_t id) {
        this->id = id;
    }

    inline Student::Student(model::PersonName name, int class_number, Date birth_date) :
        name_(std::move(name)),
        class_(class_number),
        birth_date_(birth_date) {
        if (class_number < 1 || class_number > 11)
            throw std::invalid_argument("Класс должен быть от 1 до 11");
        if (!birth_date)
            throw std::invalid_argument("Дата рождения не указана");
    }

    inline Student::Student(const Student &other) = default;

    inline Student::Student() : class_(0) {}
}

#endif //STUDENT_H
