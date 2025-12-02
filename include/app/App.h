//
// Created by sphdx on 5/18/25.
//

#ifndef APP_H
#define APP_H

#include <cstring>
#include <iostream>
#include <string>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../Slog.h"
#include "../repository/SchoolRepo.h"
#include "../utils/PathResolver.h"
#include "ui/layout/Layout.h"

class Student;

namespace app {
    class App {
        static std::string to_key(const model::PersonName &pn, const std::string &birth_date);

        repo::SchoolRepo *repo_ = nullptr;

        static constexpr size_t kPathBuf = 260;
        static constexpr size_t kCapBuf = 10;
        char students_path_buf_[kPathBuf]{};
        char grades_path_buf_[kPathBuf]{};
        char hash_table_cap_buf_[kPathBuf]{};
        static constexpr const char* kErrPopup = "Ошибка";
        bool open_error_popup_ = false;
        bool repo_loaded_ = false;
        bool auto_hash_cap_ = true;
        std::string load_error_;


        static void render_begin();
        void render_end() const;
        void render_startup_dialog();

        std::string students_dir_path_;
        std::string grades_dir_path_;
        std::string hash_table_cap_;

        GLFWwindow *window_ = nullptr;

        bool is_system_theme_dark() const;
    public:
        App();
        ~App();

        void run();
        void stop() const;
        [[nodiscard]] bool should_close() const;
    };

    inline std::string App::to_key(const model::PersonName &pn, const std::string &birth_date) {
        if (birth_date.empty()) {
            throw std::invalid_argument("Дата рождения не указана");
        }
        return pn.to_string() + " " + birth_date;
    }

    inline void App::render_startup_dialog() {
        ImGui::Begin("Загрузка справочников школы", nullptr, ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginPopupModal(kErrPopup, nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysUseWindowPadding)) {
            ImGui::Text("%s", load_error_.c_str());
            ImGui::Spacing();
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                load_error_.clear();
            }
            ImGui::EndPopup();
        }

        ImGui::InputTextWithHint("Файл студентов",
                                 "students.txt",
                                 students_path_buf_, kPathBuf);

        ImGui::InputTextWithHint("Файл оценок",
                                 "grades.txt",
                                 grades_path_buf_, kPathBuf);

        ImGui::Checkbox("Определить ёмкость автоматически", &auto_hash_cap_);

        if (!auto_hash_cap_) {
            ImGui::InputTextWithHint("Ёмкость хеш-таблицы",
                                     "100",
                                     hash_table_cap_buf_, kCapBuf);
        }

        if (open_error_popup_) {
            ImGui::OpenPopup(kErrPopup);
            open_error_popup_ = false;
        }

        if (ImGui::Button("Загрузить")) {
            load_error_.clear();
            try {
                size_t cap = 0;
                if (!auto_hash_cap_) {
                    if (std::strlen(hash_table_cap_buf_) == 0)
                        throw std::invalid_argument("Введите ёмкость хеш-таблицы или"
                                                    " поставьте флаг автоматического определения");

                    cap = std::stoul(hash_table_cap_buf_);
                    if (cap < 4 || cap > 100000)
                        throw std::invalid_argument("Ёмкость должна быть > 3 и < 100000");
                }

                repo_ = new repo::SchoolRepo(
                    students_path_buf_,
                    grades_path_buf_,
                    to_key, cap);

                repo_loaded_ = true;
                Slog::info("Справочники загружены",
                           Slog::opt("студенты", repo_->student_repo_size()),
                           Slog::opt("оценки", repo_->grade_repo_size()));
            } catch (const std::exception &e) {
                if (repo_) {
                    delete repo_;
                    repo_ = nullptr;
                }
                load_error_ = e.what();
                open_error_popup_ = true;
            }
        }

        ImGui::End();
    }


    inline App::App() {
        const std::string def_students =
            utils::PathResolver().resolve("../../resources/dirs/students.txt");
        const std::string def_grades =
            utils::PathResolver().resolve("../../resources/dirs/grades.txt");

        std::strncpy(students_path_buf_, def_students.c_str(), kPathBuf - 1);
        students_path_buf_[kPathBuf - 1] = '\0';
        std::strncpy(grades_path_buf_, def_grades.c_str(), kPathBuf - 1);
        grades_path_buf_[kPathBuf - 1] = '\0';

        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        window_ = glfwCreateWindow(mode->width, mode->height, "Школа", nullptr, nullptr);
        if (!window_) throw std::runtime_error("Failed to create GLFW window");

        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init();

        ImGuiIO &io = ImGui::GetIO();

        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
        builder.AddText("│└├─┐┌┘┬┴┼╮╭");

        static ImVector<ImWchar> ranges;
        builder.BuildRanges(&ranges);

        io.Fonts->AddFontFromFileTTF(
            utils::PathResolver().resolve("../../resources/fonts/JetBrainsMono.ttf").c_str(),
            16.0f, nullptr, ranges.Data);

        ImGui_ImplOpenGL3_CreateFontsTexture();

        ImGui::StyleColorsLight();

        Slog::info("Приложение запущено");
    }

    inline App::~App() {
        if (repo_) {
            delete repo_;
            repo_ = nullptr;
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (window_) {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }
    }

    inline void App::run() {
        render_begin();

        if (!repo_loaded_)
            render_startup_dialog();
        else
            ui::layout::render_ui(*repo_, to_key);

        render_end();
    }

    inline void App::stop() const {
        if (repo_loaded_ && repo_) {
            repo_->save_student_repo(students_path_buf_);
            repo_->save_grade_repo(grades_path_buf_);
        }
    }

    inline bool App::should_close() const {
        return glfwWindowShouldClose(window_);
    }

    inline void App::render_begin() {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    inline void App::render_end() const {
        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window_, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
    }
}

#endif //APP_H
