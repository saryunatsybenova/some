//
// Created by sphdx on 7/8/25.
//

#ifndef STUDENTGRADETABLE_H
#define STUDENTGRADETABLE_H

#include "imgui.h"
#include "app/ui/state/StudentGradeState.h"
#include "model/StudentGrade.h"

namespace app::ui::table {
    inline void student_grade_table(Vector<model::StudentGrade> &arr, state::StudentGradeState &state) {
        const size_t pages = (arr.size() + state::StudentGradeState::kPageSize - 1) / state::StudentGradeState::kPageSize;
        if (pages > 1) {
            if (ImGui::Button("<<") && state.current_page) --state.current_page;
            ImGui::SameLine();
            ImGui::Text("Стр. %zu / %zu", state.current_page + 1, pages);
            ImGui::SameLine();
            if (ImGui::Button(">>") && state.current_page + 1 < pages) ++state.current_page;
        }

        const size_t first = state.current_page * state::StudentGradeState::kPageSize;
        const size_t last = std::min(first + state::StudentGradeState::kPageSize, arr.size());

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                          ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("StudentGradeTable", 6, flags)) {
            ImGui::TableSetupColumn("Студент", ImGuiTableColumnFlags_WidthStretch, 2.f);
            ImGui::TableSetupColumn("Класс", ImGuiTableColumnFlags_WidthStretch, 1.f);
            ImGui::TableSetupColumn("Дата рождения", ImGuiTableColumnFlags_WidthStretch, 1.5f);
            ImGui::TableSetupColumn("Предмет", ImGuiTableColumnFlags_WidthStretch, 2.f);
            ImGui::TableSetupColumn("Оценка", ImGuiTableColumnFlags_WidthStretch, 1.f);
            ImGui::TableSetupColumn("Дата оценки", ImGuiTableColumnFlags_WidthStretch, 1.5f);
            ImGui::TableHeadersRow();

            for (size_t i = first; i < last; ++i) {
                const auto &sg = arr[i];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", sg.get_student_name().to_string().c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", sg.get_class().c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", sg.get_birth_date().to_string().c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", sg.get_subject().c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", sg.get_grade());
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", sg.get_grade_date().to_string().c_str());
            }

            ImGui::EndTable();
        }
    }
}

#endif //STUDENTGRADETABLE_H