//
// Created by sphdx on 7/7/25.
//

#ifndef GRADEUISTATE_H
#define GRADEUISTATE_H

#include <string>

#include "model/Grade.h"

namespace app::ui::state {
    struct GradeState {
        static constexpr size_t kNameBuf = 128;
        static constexpr size_t kSubjectBuf = 64;
        static constexpr size_t kDateBuf = 32;
        static constexpr size_t kPageSize = 20;

        // поп-апы
        bool open_add{};
        bool open_del{};
        bool open_search{};

        // ошибки
        bool add_err{};
        bool del_err{};
        bool del_none_err{};
        bool search_err{};

        std::string err_details;

        // поиск / кеш
        bool search_active{};
        std::string search_key;
        Vector<model::Grade> cached;
        size_t step_counter;

        // ввод
        char student_name[kNameBuf] = "";
        char student_birth_date[kDateBuf] = "";
        char subject[kSubjectBuf] = "";
        int grade = 5;
        char date[kDateBuf] = "";

        // пагинация
        size_t current_page{};
        bool cached_found;
    };
}

#endif //GRADEUISTATE_H