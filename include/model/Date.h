//
// Created by sphdx on 6/22/25.
//

#ifndef DATE_H
#define DATE_H
#include <cstdint>
#include <iomanip>
#include <stdexcept>

namespace model {
    enum class Month : std::uint8_t {
        Jan = 1, Feb, Mar, Apr, May, Jun,
        Jul, Aug, Sep, Oct, Nov, Dec
    };

    inline const char* month_to_str(Month m) {
        static const char* names[] = {
            "err", "jan","feb","mar","apr","may","jun",
            "jul","aug","sep","oct","nov","dec"
        };
        return names[static_cast<int>(m)];
    }

    inline Month str_to_month(const std::string& s) {
        static const std::pair<const char*, Month> map[] = {
            {"jan", Month::Jan}, {"feb", Month::Feb}, {"mar", Month::Mar},
            {"apr", Month::Apr}, {"may", Month::May}, {"jun", Month::Jun},
            {"jul", Month::Jul}, {"aug", Month::Aug}, {"sep", Month::Sep},
            {"oct", Month::Oct}, {"nov", Month::Nov}, {"dec", Month::Dec},
        };
        for (auto [txt, val] : map)
            if (s == txt) return val;
        throw std::invalid_argument("Некорректный месяц");
    }

    class Date {
        std::uint8_t  day_{0}; // 1-31
        Month         month_{Month::Jan};
        std::uint16_t year_{0};

    public:
        Date() = default;
        Date(const std::uint8_t d, const Month m, const std::uint16_t y) : day_(d), month_(m), year_(y) {}

        bool operator<(const Date& o) const;
        bool operator>(const Date& o) const;
        bool operator==(const Date& o) const;
        bool operator!() const;
        bool operator!=(const Date& o) const;
        bool operator<=(const Date& o) const;
        bool operator>=(const Date& o) const;

        [[nodiscard]] std::string to_string() const;
        friend std::ostream& operator<<(std::ostream& os, const Date& d);

        static Date parse(const std::string& str);

        [[nodiscard]] std::uint16_t year() const;
    };

    inline std::ostream & operator<<(std::ostream &os, const Date &d) {
        return os << d.to_string();
    }

    inline bool Date::operator<(const Date &o) const {
        if (year_  != o.year_)   return year_  < o.year_;
        if (month_ != o.month_)  return month_ < o.month_;
        return day_ < o.day_;
    }

    inline bool Date::operator>(const Date &o) const {
        return o < *this;
    }

    inline bool Date::operator==(const Date &o) const {
        return day_ == o.day_ && month_ == o.month_ && year_ == o.year_;
    }

    inline bool Date::operator!() const {
        return year_ == 0;
    }

    inline bool Date::operator!=(const Date &o) const {
        return !(*this == o);
    }

    inline bool Date::operator<=(const Date &o) const {
        return !(*this > o);
    }

    inline bool Date::operator>=(const Date &o) const {
        return !(*this < o);
    }

    inline std::string Date::to_string() const {
        std::ostringstream os;
        os << std::setfill('0') << std::setw(2) << static_cast<int>(day_) << ' '
           << month_to_str(month_) << ' '
           << year_;
        return os.str();
    }

    inline Date Date::parse(const std::string &str) {
        std::istringstream iss(str);
        std::string day_s, mon_s;
        std::uint16_t year;

        if (!(iss >> day_s >> mon_s >> year))
            throw std::invalid_argument("Неверный формат даты");

        if (day_s.size() > 2 || day_s.empty() ||
            day_s.size() == 1 && !std::isdigit(day_s[0]) ||
            day_s.size() == 2 && !std::isdigit(day_s[0]) && !std::isdigit(day_s[1]))
            throw std::invalid_argument("Некорректный день");

        const auto day = static_cast<std::uint8_t>(std::stoi(day_s));
        if (day == 0 || day > 31)
            throw std::invalid_argument("Некорректный день");

        for (auto& c : mon_s) c = static_cast<char>(std::tolower(c));
        const Month month = str_to_month(mon_s);

        if (year < 0 || year > 2025)
            throw std::invalid_argument("Некорректный год");

        return Date{day, month, year};
    }

    inline std::uint16_t Date::year() const {
        return year_;
    }
}

#endif //DATE_H
