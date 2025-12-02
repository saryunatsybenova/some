//
// Created by sphdx on 7/7/25.
//

#ifndef GRADEPOPUP_H
#define GRADEPOPUP_H
#include "imgui.h"
#include "app/ui/modal/CommonModal.h"
#include "app/ui/state/GradeState.h"
#include "repository/SchoolRepo.h"

namespace app::ui::pop_up {
    inline void grade_add_popup(repo::SchoolRepo &repo, state::GradeState &state) {
        if (state.open_add) {
            ImGui::OpenPopup("Добавить оценку");
            state.open_add = false;
        }
        if (ImGui::BeginPopupModal("Добавить оценку", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);
            ImGui::InputText("ФИО студента", state.student_name, state::GradeState::kNameBuf);
            ImGui::InputText("Дата рождения (DD MMM YYYY)", state.student_birth_date, state::GradeState::kDateBuf);
            ImGui::InputText("Предмет", state.subject, state::GradeState::kSubjectBuf);
            ImGui::InputInt("Оценка", &state.grade);
            ImGui::InputText("Дата оценки (DD MMM YYYY)", state.date, state::GradeState::kDateBuf);
            if (ImGui::Button("Добавить")) {
                try {
                    auto student_birth_date = model::Date::parse(state.student_birth_date);
                    auto grade_date = model::Date::parse(state.date);

                    if (grade_date.year() - student_birth_date.year() < 6)
                        throw std::invalid_argument("Некорректная дата получения оценки");

                    auto grade = model::Grade{
                        model::PersonName::parse(state.student_name),
                        model::Date::parse(state.student_birth_date),
                        state.subject,
                        state.grade,
                        model::Date::parse(state.date)
                    };
                    const auto added = repo.add_grade(grade);
                    if (!added) {
                        state.add_err = true;
                        state.err_details = "Оценка уже существует";
                    }
                } catch (const std::invalid_argument &e) {
                    state.add_err = true;
                    state.err_details = e.what();
                } catch (const std::overflow_error &e) {
                    state.add_err = true;
                    state.err_details = e.what();
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Отмена")) ImGui::CloseCurrentPopup();
        }
        modal::error_modal("Ошибка добавления", state.add_err, state.err_details.c_str());
    }

    inline void grade_del_popup(repo::SchoolRepo &repo, state::GradeState &state,
                                      const repo::ToKey to_key) {
        if (state.open_del) {
            ImGui::OpenPopup("Удалить оценку");
            state.open_del = false;
        }
        if (ImGui::BeginPopupModal("Удалить оценку", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);
            ImGui::InputText("ФИО студента", state.student_name, state::GradeState::kNameBuf);
            ImGui::InputText("Дата рождения (DD MMM YYYY)", state.student_birth_date, state::GradeState::kDateBuf);
            ImGui::InputText("Предмет", state.subject, state::GradeState::kSubjectBuf);
            ImGui::InputInt("Оценка", &state.grade);
            ImGui::InputText("Дата оценки (DD MMM YYYY)", state.date, state::GradeState::kDateBuf);
            if (ImGui::Button("Удалить")) {
                try {
                    const auto deleted = repo.del_grade(
                        model::Grade(
                            model::PersonName::parse(state.student_name),
                            model::Date::parse(state.student_birth_date),
                            state.subject,
                            state.grade,
                            model::Date::parse(state.date)
                        ));
                    if (!deleted) {
                        state.del_err = true;
                        state.err_details = "Оценка не найдена";
                    };
                } catch (const std::invalid_argument &e) {
                    state.del_err = true;
                    state.err_details = e.what();
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Отмена")) ImGui::CloseCurrentPopup();
        }
        modal::error_modal("Ошибка удаления", state.del_err, state.err_details.c_str());
    }

    inline void grade_search_popup(repo::SchoolRepo &repo, state::GradeState &state,
                                         const repo::ToKey to_key) {
        if (state.open_search) {
            ImGui::OpenPopup("Поиск оценки");
            state.open_search = false;
        }
        if (ImGui::BeginPopupModal("Поиск оценки", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            modal::ModalGuard guard(true);
            ImGui::InputText("ФИО студента", state.student_name, state::GradeState::kNameBuf);
            ImGui::InputText("Дата рождения (DD MMM YYYY)", state.student_birth_date, state::GradeState::kDateBuf);
            if (ImGui::Button("Найти")) {
                try {
                    state.step_counter = 0;
                    state.search_key = to_key(
                        model::PersonName::parse(state.student_name),
                        state.student_birth_date
                    );

                    state.cached = repo.search_grades(state.search_key, state.step_counter);
                    state.cached_found = !state.cached.empty();
                    state.search_active = true;
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

#endif //GRADEPOPUP_H