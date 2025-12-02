//
// Created by sphdx on 7/7/25.
//

#ifndef MODALS_H
#define MODALS_H
#include "imgui.h"

namespace app::ui::modal {
    struct ModalGuard {
        bool open;
        explicit ModalGuard(const bool o) : open(o) {}
        ~ModalGuard() { if (open) ImGui::EndPopup(); }
    };


    inline void error_modal(const char *title, bool &trigger_flag, const char *msg = "Некорректные данные") {
        if (trigger_flag) {
            ImGui::OpenPopup(title);
            trigger_flag = false;
        }
        if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ModalGuard guard(true);
            ImGui::Text("%s", msg);
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        }
    }
}

#endif //MODALS_H
