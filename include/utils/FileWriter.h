//
// Created by sphdx on 5/18/25.
//

#ifndef FILEWRITER_H
#define FILEWRITER_H
#include <format>
#include <fstream>
#include <string>
#include "vector/Vector.h"

namespace utils {
    class FileWriter {
        FileWriter() = default;
    public:
        static void write_file(const std::string &file_path, const std::string &to_write);

        template<typename T>
        static void write_array(const std::string &file_path, Vector<T> arr, size_t count);
    };

    template<typename T>
    void FileWriter::write_array(const std::string &file_path, Vector<T> arr, const size_t count) {
        std::ofstream file(file_path);
        if (!file) throw std::runtime_error(std::format("Не удалось открыть файл '{}'", file_path));
        for (size_t i = 0; i < count; ++i) {
            file << arr[i].to_string();
            if (i + 1 != count) file << '\n';
        }
    }

    inline void FileWriter::write_file(const std::string &file_path, const std::string &to_write) {
        std::ofstream file(file_path);
        if (!file) {
            throw std::runtime_error(
                std::format("Could not open file '{}'", file_path)
                );
        }
        file << to_write;
        file.close();
    }
}
#endif //FILEWRITER_H
