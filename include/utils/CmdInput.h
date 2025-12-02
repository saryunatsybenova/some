//
// Created by sphdx on 06.04.2025.
//

#ifndef CMDINPUT_H
#define CMDINPUT_H

#include <iostream>
#include <string>
#include <numeric>

namespace utils {
    class CmdInput {
    public:
        static std::string get_line();

        static int get_number();
    };

    inline std::string CmdInput::get_line() {
        std::string user_input;
        std::getline(std::cin, user_input);
        return user_input;
    }

    inline int CmdInput::get_number() {
        int user_input;
        std::cin >> user_input;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return user_input;
    }
}

#endif //CMDINPUT_H