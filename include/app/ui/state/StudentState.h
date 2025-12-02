//
// Created by sphdx on 7/7/25.
//

#ifndef STUDENTUISTATE_H
#define STUDENTUISTATE_H
#include "model/Student.h"
#include <string>

namespace app::ui::state {
    struct StudentState {
        static constexpr size_t kNameBuf = 128;
        static constexpr size_t kClassBuf = 16;
        static constexpr size_t kDateBuf = 32;
        static constexpr size_t kPageSize = 20;

        // поп-апы
        bool open_add{};
        bool open_del{};
        bool open_search{};

        // ошибки
        bool add_err{};
        bool del_err{};
        bool search_err{};

        std::string err_details;

        // поиск
        bool search_active{};
        std::string search_key;
        Vector<model::Student> cached;
        bool cached_found{};
        size_t step_counter;

        // ввод
        char name[kNameBuf] = "";
        char class_name[kClassBuf] = "";
        char birth_date[kDateBuf] = "";

        // пагинация
        size_t current_page{};
    };
}
#endif //STUDENTUISTATE_H