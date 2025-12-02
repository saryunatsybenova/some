//
// Created by sphdx on 6/23/25.
//

#ifndef UTILS_H
#define UTILS_H

#include <iostream>

namespace utils {
    template<typename T>
    void print_array(T arr, const size_t size, char delim = ',') {
        if (size < 1) return;
        std::cout << arr[0];
        for (size_t i = 1; i < size; ++i) {
            std::cout << delim << arr[i];
        }
        std::cout << std::endl;
    }
}

#endif //UTILS_H
