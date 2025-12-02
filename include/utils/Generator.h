//
// Created by sphdx on 05.03.2025.
//

#ifndef GENERATOR_H
#define GENERATOR_H
#include <string>
#include <unordered_map>
#include <random>
#include <cmath>
#include <locale>
#include <codecvt>
#include <fstream>

#include "../model/Application.h"
#include "../model/Car.h"
#include "../model/LicensePlate.h"
#include "../model/PersonName.h"
namespace utils {
    class Generator {
        std::mt19937 gen_;

    public:
        enum class Lang {
            ENGLISH,
            RUSSIAN,
        };

        enum class StringType {
            FIRST_UPPERCASE,
            LAST_UPPERCASE,
            UPPERCASE,
            LOWERCASE,
        };

        std::pmr::unordered_map<Lang, std::pair<char32_t, char32_t> > lang_upper_map_ = {
            {Lang::ENGLISH, {U'A', U'Z'}},
            {Lang::RUSSIAN, {U'А', U'Я'}},
        };

        std::pmr::unordered_map<Lang, std::pair<char32_t, char32_t> > lang_lower_map_ = {
            {Lang::ENGLISH, {U'a', U'z'}},
            {Lang::RUSSIAN, {U'а', U'я'}},
        };

        explicit Generator();

        std::string gen_random_upper_char(Lang lang = Lang::ENGLISH);

        std::string gen_random_lower_char(Lang lang = Lang::ENGLISH);

        int gen_random_int(const int &min, const int &max);

        std::string gen_random_word(const int &len_max, const int &len_min = 3, Lang lang = Lang::ENGLISH,
                                    StringType type = StringType::FIRST_UPPERCASE);

        LicensePlate gen_random_license_plate(Lang lang = Lang::ENGLISH);

        PersonName gen_random_person_name(Lang lang = Lang::ENGLISH);

        Car gen_random_car(Lang lang = Lang::ENGLISH);

        Application gen_random_application(int min, int max);

        template<typename Type>
        void gen_field(std::string &result, Lang lang = Lang::ENGLISH);

        template<typename... Types>
        std::string gen_random_string(Lang lang = Lang::ENGLISH);

        template<typename... Types>
        void generate_file(const std::string &file_path, int rows_to_gen, Lang lang = Lang::ENGLISH);

        static std::string to_utf8(char32_t ch);
    };

    inline std::string Generator::gen_random_upper_char(const Lang lang) {
        std::uniform_int_distribution dis(lang_upper_map_[lang].first, lang_upper_map_[lang].second);
        return to_utf8(dis(gen_));
    }

    inline std::string Generator::gen_random_lower_char(const Lang lang) {
        std::uniform_int_distribution dis(lang_lower_map_[lang].first, lang_lower_map_[lang].second);
        return to_utf8(dis(gen_));
    }

    inline int Generator::gen_random_int(const int &min, const int &max) {
        std::uniform_int_distribution dis(min, max);
        return dis(gen_);
    }

    inline std::string Generator::gen_random_word(const int &len_max, const int &len_min,
                                                  const Lang lang,
                                                  const StringType type) {
        const size_t word_length = std::max<size_t>(1, std::uniform_int_distribution(len_min, len_max)(gen_));
        std::string word;

        if (type == StringType::FIRST_UPPERCASE) word += gen_random_upper_char(lang);

        for (size_t i = 0 + (type == StringType::FIRST_UPPERCASE);
             i < word_length - (type == StringType::LAST_UPPERCASE); ++i) {
            const auto symbol = type != StringType::UPPERCASE
                                    ? gen_random_lower_char(lang)
                                    : gen_random_upper_char(lang);
            word += symbol;
             }

        if (type == StringType::LAST_UPPERCASE) word += gen_random_upper_char(lang);

        return word;
    }

    inline LicensePlate Generator::gen_random_license_plate(const Lang lang) {
        return LicensePlate{
            gen_random_upper_char(lang),
            gen_random_int(100, 999),
            gen_random_upper_char(lang),
            gen_random_upper_char(lang),
        };
    }

    inline PersonName Generator::gen_random_person_name(const Lang lang) {
        return PersonName{
            gen_random_word(7, 3, lang),
            gen_random_word(7, 3, lang),
            gen_random_word(7, 3, lang)
        };
    }

    inline Car Generator::gen_random_car(const Lang lang) {
        return Car{
            gen_random_word(3, 3, lang),
            gen_random_word(7, 3, lang),
        };
    }

    inline Application Generator::gen_random_application(const int min = 0, const int max = std::numeric_limits<int>::max()) {
        return Application{
            static_cast<size_t>(gen_random_int(min, max)),
            };
    }


    template<typename Type>
    void Generator::gen_field(std::string &result, const Lang lang) {
        if constexpr (std::is_same_v<Type, LicensePlate>) {
            result += gen_random_license_plate(lang).to_string();
        } else if constexpr (std::is_same_v<Type, PersonName>) {
            result += gen_random_person_name(lang).to_string();
        } else if constexpr (std::is_same_v<Type, Car>) {
            result += gen_random_car(lang).to_string();
        } else if constexpr (std::is_same_v<Type, Application>) {
            result += gen_random_application().to_string();
        }
    }

    template<typename... Types>
    std::string Generator::gen_random_string(const Lang lang) {
        std::string result;
        ((gen_field<Types>(result, lang), result += "\t"), ...);
        return result;
    }

    template<typename... Types>
    void Generator::generate_file(const std::string &file_path, const int rows_to_gen, const Lang lang) {
        std::ofstream file(file_path);
        if (!file) {
            throw std::runtime_error("couldn't open file " + file_path);
        }

        for (size_t row = 0; row < rows_to_gen; ++row) {
            file << gen_random_string<Types...>(lang) << std::endl;
        }

        file.close();
    }

    inline std::string Generator::to_utf8(const char32_t ch) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.to_bytes(ch);
    }

    inline Generator::Generator() : gen_([] {
        std::random_device rd;
        std::seed_seq seq{rd(), rd(), rd(), rd(), rd()};
        return std::mt19937(seq);
    }()) {
    }
}


#endif //GENERATOR_H
