//
// Created by sphdx on 6/23/25.
//

#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>

namespace app {
    void push_gui_log(const std::string &line);

    inline std::vector<std::string> &get_log_entries() {
        static std::vector<std::string> entries;
        return entries;
    }

    inline void push_gui_log(const std::string &line) {
        get_log_entries().push_back(line);
    }
}

#endif //LOG_H
