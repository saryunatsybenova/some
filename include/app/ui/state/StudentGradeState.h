//
// Created by sphdx on 7/8/25.
//

#ifndef STUDENTGRADESTATE_H
#define STUDENTGRADESTATE_H
#include "model/StudentGrade.h"

namespace app::ui::state {
    struct StudentGradeState {
        static constexpr size_t kNameBuf = 128;
        static constexpr size_t kDateBuf = 32;
        static constexpr size_t kClassBuf = 32;
        static constexpr size_t kPathBuf = 256;
        static constexpr size_t kPageSize = 20;

        char birth_date[kNameBuf] = "";
        char start[kDateBuf] = "";
        char end[kDateBuf] = "";
        char path[kPathBuf] = "";
        char subject[kClassBuf] = "";

        // результаты
        Vector<model::StudentGrade> filtered;
        bool filter_applied = false;
        size_t step_counter;

        // неверный ввод
        bool filter_err{};
        std::string err_details;

        // пусто / путь неверен
        bool save_err{};
        bool open_save_dialog{};

        // пагинация
        size_t current_page{};
    };
}

#endif //STUDENTGRADESTATE_H