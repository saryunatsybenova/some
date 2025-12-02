//
// Created by sphdx on 7/8/25.
//

#ifndef STUDENTGRADEPOPUP_H
#define STUDENTGRADEPOPUP_H
#include "imgui.h"
#include "app/ui/modal/CommonModal.h"
#include "app/ui/state/StudentGradeState.h"
#include "repository/SchoolRepo.h"

namespace app::ui::pop_up {
    inline void save_dialog_popup(repo::SchoolRepo &repo, state::StudentGradeState &state) {
        if (state.open_save_dialog) {
            ImGui::OpenPopup("Сохранить в файл");
            state.open_save_dialog = false;
        }
        if (ImGui::BeginPopupModal("Сохранить в файл", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);

            ImGui::InputText("Путь к файлу",
                             state.path,
                             state::StudentGradeState::kPathBuf);
            if (ImGui::Button("Сохранить")) {
                try {
                    repo.save_filtered(
                        state.path, state.filtered, state.filtered.size()
                    );
                } catch (const std::runtime_error &e) {
                    state.save_err = true;
                    state.err_details = e.what();
                } catch (...) {
                    state.save_err = true;
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Отмена")) ImGui::CloseCurrentPopup();
        }

        modal::error_modal("Ошибка фильтра", state.filter_err, state.err_details.c_str());
        modal::error_modal("Ошибка сохранения", state.save_err, "Таблица пуста, сохранять нечего");
    }
}

#endif //STUDENTGRADEPOPUP_H