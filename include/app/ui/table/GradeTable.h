//
// Created by sphdx on 7/7/25.
//

#ifndef GRADETABLE_H
#define GRADETABLE_H
#include "imgui.h"
#include "app/ui/modal/CommonModal.h"
#include "app/ui/state/GradeState.h"
#include "model/Grade.h"

namespace app::ui::table {
    inline void grade_table(const Vector<model::Grade> &arr, state::GradeState &state) {
        if (state.search_active) {
            if (!state.cached_found) {
                ImGui::Text("Ничего не найдено.");
                return;
            }

            if (state.step_counter != 0) {
                ImGui::Text("Поиск выполнен, шаги: %zu.", state.step_counter);
            }
        }

        const size_t pages = (arr.size() + state::GradeState::kPageSize - 1) / state::GradeState::kPageSize;
        if (pages > 1) {
            if (ImGui::Button("<<") && state.current_page > 0) --state.current_page;
            ImGui::SameLine();
            ImGui::Text("Стр. %zu / %zu", state.current_page + 1, pages);
            ImGui::SameLine();
            if (ImGui::Button(">>") && state.current_page + 1 < pages) ++state.current_page;
        }

        // TODO: проверить ломается ли пагинация: 2 листа, удаляем элемент, становится 1 лист

        const size_t first = state.current_page * state::GradeState::kPageSize;
        const size_t last = std::min(first + state::GradeState::kPageSize, arr.size());

        if (first >= arr.size())
            state.current_page = 0;

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                          ImGuiTableFlags_SizingStretchProp;

        if (ImGui::BeginTable("GradesTable", 5, flags)) {
            ImGui::TableSetupColumn("Студент", ImGuiTableColumnFlags_WidthStretch, 2.3f);
            ImGui::TableSetupColumn("Дата рождения", ImGuiTableColumnFlags_WidthStretch, 1.5f);
            ImGui::TableSetupColumn("Предмет", ImGuiTableColumnFlags_WidthStretch, 2.f);
            ImGui::TableSetupColumn("Оценка", ImGuiTableColumnFlags_WidthStretch, 1.f);
            ImGui::TableSetupColumn("Дата оценки", ImGuiTableColumnFlags_WidthStretch, 1.5f);
            ImGui::TableHeadersRow();

            for (size_t i = first; i < last; ++i) {
                const auto &g = arr[i];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", g.get_student_name().to_string().c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", g.get_student_birth_date().to_string().c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", g.get_subject().c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", g.get_grade());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%s", g.get_date().to_string().c_str());
            }

            ImGui::EndTable();
        }
    }
}

#endif //GRADETABLE_H