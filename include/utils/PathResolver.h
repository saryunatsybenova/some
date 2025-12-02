//
// Created by sphdx on 03.03.2025.
//

#ifndef PATH_H
#define PATH_H

#include <string>
#include <filesystem>

namespace utils {
    class PathResolver {
        std::filesystem::path baseDir_;
    public:
        // для релиза убирать "/ .."
        explicit PathResolver(std::filesystem::path baseDir = std::filesystem::current_path() / "..")
            : baseDir_(std::move(baseDir)) {
        }

        [[nodiscard]] std::string resolve(const std::string &relativePath) const {
            const auto full = (baseDir_ / relativePath).lexically_normal();
            return full.string();
        }

        [[nodiscard]] bool exists(const std::string &relativePath) const {
            return std::filesystem::exists(baseDir_ / relativePath);
        }
    };
}


#endif //PATH_H
