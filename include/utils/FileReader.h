//
// Created by sphdx on 06.04.2025.
//

#ifndef FILEREADER_H
#define FILEREADER_H
#include <fstream>
#include <string>

#include "../Slog.h"
#include "vector/Vector.h"

namespace utils {
    template<typename T, typename... Types>
    concept ComplexType = requires(T t, std::string str, int counter)
    {
        t.parse(str, counter);
    };

    template<typename T>
    concept Parsable = requires(const std::string &s)
    {
        { T::parse(s) } -> std::same_as<T>;
    };

    class FileReader {
        FileReader() = default;

    public:
        template<Parsable T>
        static Vector<T> read_file(const std::string &file_path, size_t& out_size);
    };

    template<Parsable T>
    Vector<T> FileReader::read_file(const std::string &file_path, size_t &out_size) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            if (std::ofstream new_file(file_path); !new_file.is_open()) // вдруг нет прав?
                throw std::runtime_error("can't create file");
            out_size = 0;
            return Vector<T>();
        }
        out_size = 0;
        std::string line;
        while (std::getline(file, line))
            if (!line.empty())
                ++out_size;

        if (out_size == 0)
            return Vector<T>();

        file.clear();
        file.seekg(0);

        Vector<T> arr{};
        std::size_t idx = 0;

        while (idx < out_size && std::getline(file, line)) {
            if (line.empty()) continue;
            arr.push_back(T::parse(line));
        }
        return arr;
    }
}

#endif //FILEREADER_H
