//
// Created by sphdx on 7/8/25.
//

#ifndef LAYOUT_H
#define LAYOUT_H
#include "imgui.h"
#include "app/ui/pop-up/GradePopUp.h"
#include "app/ui/pop-up/StudentPopUp.h"
#include "app/ui/pop-up/StudentGradePopUp.h"
#include "app/ui/state/GradeState.h"
#include "app/ui/state/StudentState.h"
#include "app/ui/state/StudentGradeState.h"
#include "app/ui/table/GradeTable.h"
#include "app/ui/table/StudentTable.h"
#include "app/ui/table/StudentGradeTable.h"
#include "repository/SchoolRepo.h"

namespace app::ui::layout {
    inline void render_students_dir(state::StudentState &state, repo::SchoolRepo &repo,
                                   const repo::ToKey to_key) {
        // контекстное меню
        if (ImGui::BeginPopupContextWindow("student_ctx_menu",
                                           ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Добавить")) state.open_add = true;
            if (ImGui::MenuItem("Удалить")) state.open_del = true;
            if (ImGui::MenuItem("Поиск")) state.open_search = true;
            if (ImGui::MenuItem("Сбросить")) state.search_active = false;
            ImGui::EndPopup();
        }

        pop_up::student_add_popup(repo, state);
        pop_up::student_del_popup(repo, state, to_key);
        pop_up::student_search_popup(repo, state, to_key);

        Vector<model::Student> students;
        size_t count = 0;
        if (state.search_active && state.cached_found) {
            students = state.cached;
            count = 1;
        } else {
            students = repo.students();
            count = repo.student_repo_size();
        }

        if (!students.empty() && count) {
            table::student_table(students, state);
        } else if (state.search_active) {
            ImGui::Text("Студент не найден.");
        }
    }

    inline void render_grades_dir(ui::state::GradeState &state, repo::SchoolRepo &repo,
                                        const repo::ToKey to_key) {
        // контекстное меню
        if (ImGui::BeginPopupContextWindow("grade_ctx_menu",
                                           ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Добавить")) state.open_add = true;
            if (ImGui::MenuItem("Удалить")) state.open_del = true;
            if (ImGui::MenuItem("Поиск")) state.open_search = true;
            if (ImGui::MenuItem("Сбросить")) state.search_active = false;
            ImGui::EndPopup();
        }

        pop_up::grade_add_popup(repo, state);
        pop_up::grade_del_popup(repo, state, to_key);
        pop_up::grade_search_popup(repo, state, to_key);

        Vector<model::Grade> grades;
        size_t count = 0;
        if (state.search_active && state.cached_found) {
            grades = state.cached;
            count = state.cached.size();
        } else {
            grades = repo.grades();
            count = repo.grade_repo_size();
        }

        if (!grades.empty() && count) {
            table::grade_table(grades, state);
        } else if (state.search_active) {
            ImGui::Text("Оценки не найдены.");
        }
    }

    inline void render_filtered_view(state::StudentGradeState &state, repo::SchoolRepo &repo) {
        if (ImGui::BeginPopupContextWindow("filter_ctx_menu", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Сбросить")) {
                state = {};
            }
            if (ImGui::MenuItem("Сохранить")) {
                state.open_save_dialog = true;
            }
            ImGui::EndPopup();
        }

        ImGui::InputText("Дата рождения ученика", state.birth_date, state::StudentGradeState::kNameBuf);
        ImGui::InputText("Предмет", state.subject, state::StudentGradeState::kClassBuf);
        ImGui::InputText("Дата начала", state.start, state::StudentGradeState::kDateBuf);
        ImGui::InputText("Дата конца", state.end, state::StudentGradeState::kDateBuf);

        if (ImGui::Button("Фильтровать")) {
            try {
                if (state.subject == "") {
                    throw std::invalid_argument("Предмет не указан");
                }

                state.step_counter = 0;
                state.filtered = repo.get_filtered(
                    model::Date::parse(state.birth_date),
                    state.subject,
                    model::Date::parse(state.start),
                    model::Date::parse(state.end),
                    state.step_counter
                );
                if (state.filtered.empty())
                    throw std::invalid_argument("Ничего не найдено");

                state.filter_applied = true;
            } catch (const std::invalid_argument &e) {
                state.filtered = Vector<model::StudentGrade>();
                state.filter_applied = true;
                state.filter_err = true;
                state.err_details = e.what();
            } catch (...) {
                state.filtered = Vector<model::StudentGrade>();
                state.filter_applied = true;
                state.filter_err = true;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Сохранить")) {
            if (!state.filtered.empty()) state.open_save_dialog = true;
            else state.save_err = true;
        }

        pop_up::save_dialog_popup(repo, state);

        if (state.filter_applied && !state.filtered.empty())
            table::student_grade_table(state.filtered, state);
        else if (state.filter_applied)
            ImGui::Text("Ничего не найдено, возможно ошибка ввода");
    }

    inline void render_console() {
        static ImGuiTextFilter filter;
        static bool auto_scroll = true;

        ImGui::Checkbox("Автоскролл", &auto_scroll);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        filter.Draw("Фильтр");

        ImGui::SameLine();
        if (ImGui::Button("Скопировать всё")) {
            std::string combined;
            for (const auto &entry: get_log_entries()) {
                if (filter.PassFilter(entry.c_str()))
                    combined += entry + "\n";
            }
            ImGui::SetClipboardText(combined.c_str());
        }

        ImGui::Separator();

        ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        int index = 0;
        for (const auto &entry: get_log_entries()) {
            if (filter.PassFilter(entry.c_str())) {
                std::string label = entry + "##" + std::to_string(index);
                ImGui::Selectable(label.c_str());

                if (ImGui::BeginPopupContextItem(("log_item_popup_" + std::to_string(index)).c_str())) {
                    if (ImGui::MenuItem("Скопировать строку")) {
                        ImGui::SetClipboardText(entry.c_str());
                    }
                    ImGui::EndPopup();
                }
            }
            ++index;
        }

        if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
    }

    inline void render_debug_tools(const repo::SchoolRepo &repo) {
        ImGui::Text("Просмотр структур");

        ImGui::BeginChild("StructureViewButtons", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

        if (ImGui::Button("Дерево дат")) {
            Slog::info("\n" + repo.date_tree_structure());
        }

        if (ImGui::Button("Горизонтальное дерево дат")) {
            Slog::info("\n" + repo.date_tree_structure(true));
        }

        if (ImGui::Button("Дерево ключей")) {
            Slog::info("\n" + repo.key_tree_structure());
        }

        if (ImGui::Button("Горизонтальное дерево ключей")) {
            Slog::info("\n" + repo.key_tree_structure(true));
        }

        if (ImGui::Button("Хеш-таблица")) {
            Slog::info("\n" + repo.table_structure());
        }

        if (ImGui::Button("Хеш-таблица полностью")) {
            Slog::info("\n" + repo.table_structure(false));
        }

        ImGui::EndChild();
    }

    struct Child {
        explicit Child(const char *id, const ImVec2 size, const ImGuiWindowFlags flags = 0) {
            ImGui::BeginChild(id, size, true, flags | ImGuiWindowFlags_AlwaysUseWindowPadding);
        }

        ~Child() { ImGui::EndChild(); }
    };

    inline bool splitter(float thickness, bool vertical, float &first, float &second, float overall) {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        if ((!vertical && avail.x <= 0.0f) || (vertical && avail.y <= 0.0f))
            return false;

        ImVec2 size = vertical
                          ? ImVec2(thickness, ImGui::GetContentRegionAvail().y)
                          : ImVec2(ImGui::GetContentRegionAvail().x, thickness);

        if (size.x <= 0.0f) size.x = 1.0f;
        if (size.y <= 0.0f) size.y = 1.0f;

        ImGui::InvisibleButton("##splitter", size);
        if (!ImGui::IsItemActive()) return false;

        float delta = vertical
                          ? ImGui::GetIO().MouseDelta.x / overall
                          : ImGui::GetIO().MouseDelta.y / overall;

        first += delta;
        second -= delta;

        const float min_size = thickness * 4;
        if (first < min_size)  { second -= min_size - first;  first  = min_size; }
        if (second < min_size) { first  -= min_size - second; second = min_size; }

        return true;
    }

    inline void normalize(float &a, float &b, float &c) {
        const float sum = a + b + c;
        a /= sum;
        b /= sum;
        c /= sum;
    }

    struct MainLayout {
        float left = 0.45f;
        float middle = 0.55f;
        float right = 0.0f;
        float top = 0.40f;

        float top_row = 0.4f;

        constexpr static float splitter = 5.f;

        void normalize() { layout::normalize(left, middle, right); }
    };

    inline void render_ui(repo::SchoolRepo &repo, const repo::ToKey to_key) {
        using layout::Child;

        static MainLayout main_layout;
        static state::StudentState student_state;
        static state::GradeState grade_state;
        static state::StudentGradeState filter_state;

        const ImGuiViewport *vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);

        if (!ImGui::Begin("Школа", nullptr,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                          ImGuiWindowFlags_NoScrollbar)) {
            ImGui::End();
            return;
        }

        main_layout.normalize();

        constexpr float thick = MainLayout::splitter * 2;
        const float W = ImGui::GetContentRegionAvail().x;
        const float usableW = W - thick - ImGui::GetStyle().WindowPadding.x * 2.f;

        float leftW = usableW * main_layout.left;
        float rightW = usableW - leftW; {
            Child _left("LeftColumn", ImVec2(leftW, 0));

            const float availH = ImGui::GetContentRegionAvail().y - thick;
            float topRowH = availH * main_layout.top_row;
            float consoleH = availH - topRowH - thick;

            const float totalH = std::max(1.f, topRowH + consoleH + thick);

            consoleH = totalH - topRowH - thick; {
                Child _top("TopRow", ImVec2(0, topRowH));

                const float availX = ImGui::GetContentRegionAvail().x;
                float dbgW = availX * 0.35f;
                float fltW = availX - dbgW - thick; {
                    Child _("Debug", ImVec2(dbgW, 0),
                            ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoScrollWithMouse |
                            ImGuiWindowFlags_NoTitleBar);
                    render_debug_tools(repo);
                }

                ImGui::SameLine(); {
                    Child _("Filter", ImVec2(fltW, 0),
                            ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoScrollWithMouse |
                            ImGuiWindowFlags_NoTitleBar);
                    render_filtered_view(filter_state, repo);
                }
            }

            ImGui::PushID("split_top_bottom");
            splitter(MainLayout::splitter, false, topRowH, consoleH, 1.f);
            ImGui::PopID();

            if (topRowH > 0.f && consoleH > 0.f)
                main_layout.top_row = topRowH / (topRowH + consoleH + thick); {
                Child _("Console", ImVec2(0, consoleH));
                render_console();
            }
        }

        ImGui::SameLine();
        ImGui::PushID("split_v_main");
        splitter(MainLayout::splitter, true, leftW, rightW, 1.f);
        ImGui::PopID();
        ImGui::SameLine(); {
            Child _right("RightColumn", ImVec2(rightW, 0));

            const float availH = ImGui::GetContentRegionAvail().y - thick;
            float studH = availH * main_layout.top;
            float gradeH = availH - studH - thick; {
                Child _("Students", ImVec2(0, studH));
                render_students_dir(student_state, repo, to_key);
            }

            ImGui::PushID("split_h_right");
            splitter(MainLayout::splitter, false, studH, gradeH, 1.f);
            ImGui::PopID(); {
                Child _("Grades", ImVec2(0, gradeH));
                render_grades_dir(grade_state, repo, to_key);
            }

            if (studH > 0.f && gradeH > 0.f)
                main_layout.top = studH / (studH + gradeH + thick);
        }

        const float sumW = leftW + rightW;
        if (sumW > 0.f) {
            main_layout.left = leftW / sumW;
            main_layout.middle = rightW / sumW;
        }

        ImGui::End();
    }
}

#endif //LAYOUT_H