//
// Created by sphdx on 7/7/25.
//

#ifndef STUDENTUIPOPUP_H
#define STUDENTUIPOPUP_H
#include "imgui.h"
#include "app/App.h"
#include "app/ui/modal/CommonModal.h"
#include "app/ui/state/StudentState.h"
#include "repository/SchoolRepo.h"

namespace app::ui::pop_up {
    inline void student_add_popup(repo::SchoolRepo &repo, state::StudentState &state) {
        if (state.open_add) {
            ImGui::OpenPopup("Добавить ученика");
            state.open_add = false;
        }
        if (ImGui::BeginPopupModal("Добавить ученика", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);
            ImGui::InputText("ФИО", state.name, state::StudentState::kNameBuf);
            ImGui::InputText("Класс", state.class_name, state::StudentState::kClassBuf);
            ImGui::InputText("Дата рождения", state.birth_date, state::StudentState::kDateBuf);
            if (ImGui::Button("Добавить")) {
                try {
                    int class_number = std::stoi(state.class_name);
                    const auto added = repo.add_student({
                        model::PersonName::parse(state.name),
                        class_number,
                        model::Date::parse(state.birth_date)
                    });
                    if (!added) {
                        state.add_err = true;
                        state.err_details = "Ученик уже существует";
                    }
                } catch (const std::exception& e) { // inv_arg + overflow_error
                    state.add_err = true;
                    state.err_details = e.what();
                    if (std::string(e.what()).find("stoi") != std::string::npos) {
                        state.err_details = "Неверный формат номера класса";
                    }
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Отмена")) ImGui::CloseCurrentPopup();
        }
        modal::error_modal("Ошибка добавления", state.add_err, state.err_details.c_str());
    }

    inline void student_del_popup(repo::SchoolRepo &repo, state::StudentState &state, const repo::ToKey to_key) {
        if (state.open_del) {
            ImGui::OpenPopup("Удалить ученика");
            state.open_del = false;
        }
        if (ImGui::BeginPopupModal("Удалить ученика", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);
            ImGui::InputText("ФИО", state.name, state::StudentState::kNameBuf);
            ImGui::InputText("Класс", state.class_name, state::StudentState::kClassBuf);
            ImGui::InputText("Дата рождения", state.birth_date, state::StudentState::kDateBuf);
            if (ImGui::Button("Удалить")) {
                try {
                    int class_number = std::stoi(state.class_name);

                    model::Student student_to_delete(
                        model::PersonName::parse(state.name),
                        class_number,
                        model::Date::parse(state.birth_date)
                    );
                    
                    state.del_err = !repo.del_student(student_to_delete);
                    if (state.del_err) state.err_details = "Ученик не найден";
                } catch (const std::invalid_argument &e) {
                    state.del_err = true;
                    state.err_details = e.what();

                    if (std::string(e.what()).find("stoi") != std::string::npos) {
                        state.err_details = "Неверный формат номера класса";
                    }
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Отмена")) ImGui::CloseCurrentPopup();
        }
        modal::error_modal("Ошибка удаления", state.del_err, state.err_details.c_str());
    }

    inline void student_search_popup(repo::SchoolRepo &repo, state::StudentState &state, const repo::ToKey to_key) {
        if (state.open_search) {
            ImGui::OpenPopup("Поиск ученика");
            state.open_search = false;
        }
        if (ImGui::BeginPopupModal("Поиск ученика", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);
            ImGui::InputText("ФИО", state.name, state::StudentState::kNameBuf);
            ImGui::InputText("Дата рождения", state.birth_date, state::StudentState::kDateBuf);
            if (ImGui::Button("Найти")) {
                try {
                    state.search_key = to_key(model::PersonName::parse(state.name), model::Date::parse(state.birth_date).to_string());
                    state.step_counter = 0;
                    state.cached = Vector<model::Student>();
                    auto *student = repo.search_student(state.search_key, state.step_counter);
                    if (student != nullptr) {
                        state.cached.push_back(*student);
                        state.cached_found = (!state.cached.empty());
                        state.search_active = true;
                    } else {
                        state.err_details = "Ученик не найден";
                        state.search_err = true;
                    }
                } catch (const std::invalid_argument &e) {
                    state.search_err = true;
                    state.err_details = e.what();
                }

                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Отмена")) ImGui::CloseCurrentPopup();
        }
        modal::error_modal("Ошибка поиска", state.search_err, state.err_details.c_str());
    }
}

#endif //STUDENTUIPOPUP_H