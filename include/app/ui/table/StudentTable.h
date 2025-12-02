//
// Created by sphdx on 7/7/25.
//

#ifndef STUDENTTABLE_H
#define STUDENTTABLE_H

#include "imgui.h"
#include "app/ui/state/StudentState.h"
#include "model/Student.h"

namespace app::ui::table {
    inline void student_table(Vector<model::Student> &arr, state::StudentState &state) {
        if (state.search_active) {
            if (!state.cached_found) {
                ImGui::Text("Ничего не найдено.");
                return;
            }

            if (state.step_counter != 0) {
                ImGui::Text("Поиск выполнен, шаги: %zu.", state.step_counter);
            }
        }


        const size_t pages = (arr.size() + state::StudentState::kPageSize - 1) / state::StudentState::kPageSize;
        if (pages > 1) {
            if (ImGui::Button("<<") && state.current_page) --state.current_page;
            ImGui::SameLine();
            ImGui::Text("Стр. %zu / %zu", state.current_page + 1, pages);
            ImGui::SameLine();
            if (ImGui::Button(">>") && state.current_page + 1 < pages) ++state.current_page;
        }

        const size_t first = state.current_page * state::StudentState::kPageSize;
        const size_t last = std::min(first + state::StudentState::kPageSize, arr.size());

        if (first >= arr.size())
            state.current_page = 0;

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                          ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("StudentsTable", 3, flags)) {
            ImGui::TableSetupColumn("ФИО", ImGuiTableColumnFlags_WidthStretch, 4.f);
            ImGui::TableSetupColumn("Класс", ImGuiTableColumnFlags_WidthStretch, 1.f);
            ImGui::TableSetupColumn("Дата рождения", ImGuiTableColumnFlags_WidthStretch, 2.f);
            ImGui::TableHeadersRow();
            for (size_t i = first; i < last; ++i) {
                const model::Student &s = arr[i];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", s.get_name().to_string().c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", s.get_class());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", s.get_birth_date().to_string().c_str());
            }

            ImGui::EndTable();
        }
    }
}

#endif //STUDENTTABLE_H