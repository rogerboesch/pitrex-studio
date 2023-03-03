
#include "pitrex_studio_app.hpp"
#include "pitrex_project.hpp"
#include "rbbase/RBLogger.hpp"

#include <filesystem>
#include <fstream>
#include <streambuf>

#include "./imgui/imgui.h"
#ifdef CIMGUI_FREETYPE
#include "./imgui/misc/freetype/imgui_freetype.h"
#endif
#include "./imgui/imgui_internal.h"
#include "cimgui.h"

extern "C" {
    const char* textbuffer_get_line(int row);
    int textbuffer_get_rows(void);
    void platform_clear_screen(void);
}

const int ScreenWidth = 1024;
const int ScreenHeight = 768;
#define WINDOW_TITLE_BAR_HEIGHT 16

typedef struct _LINE {
    int x1, y1, x2, y2;
} LINE;

static auto vector_getter = [](void* vec, int idx, const char** out_text) {
    auto& vector = *static_cast<std::vector<std::string>*>(vec);
    if (idx < 0 || idx >= static_cast<int>(vector.size())) {
        return false;
    }
    
    *out_text = vector.at(idx).c_str();
    return true;
};

bool rbCombo(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty()) {
        return false;
    }
    
    return ImGui::Combo(label, currIndex, vector_getter, static_cast<void*>(&values), values.size());
}

bool rbListBox(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty()) {
        return false;
    }
    
    return ImGui::ListBox(label, currIndex, vector_getter, static_cast<void*>(&values), values.size(), 30);
}

PitrexStudioApp app;
static PitrexProject project;
static std::vector<std::string> consoleBuffer;
static std::vector<LINE> lineBuffer;
static bool render_flag = false;
static std::thread::id RENDER_THREAD_ID;

PitrexStudioApp::PitrexStudioApp() {
    m_calledFirstTime = true;
    auto lang = TextEditor::LanguageDefinition::SuperBasic();
    m_editor.SetLanguageDefinition(lang);
}

void PitrexStudioApp::LoadCurrentProjectFile() {
    std::string content;
 
    if (project.LoadCurrentFile(content)) {
        m_editor.SetText(content);
        auto lang = TextEditor::LanguageDefinition::Basic();
        m_editor.SetLanguageDefinition(lang);
        m_editor.SetPalette(TextEditor::GetRetroBluePalette());
    }
}

void PitrexStudioApp::Initialise() {
    std::string filepath = project.GetCurrentFile();
    LoadCurrentProjectFile();
    m_calledFirstTime = false;

    RENDER_THREAD_ID = std::this_thread::get_id();
}

void PitrexStudioApp::Frame(const float width, const float height) {
    m_width = width;
    m_height = height;

    if (m_calledFirstTime) {
        Initialise();
    }
    
    // Render views
    // +-------------+
    // +-------------+ Toolbar
    // |      |      | Left: Editor, Right: Vectrex, CPU View
    // |      +------+
    // +------+------+ Right: Console
    
    const int toolbar_height = 30;
    const int panel_margin = 5;
    const int panel_top = toolbar_height+2*panel_margin;
    const int console_height = 140;
    const int panel_width = 400;

    int editorWidth = m_width - panel_width - 2*panel_margin;
    int panel_height = m_height-toolbar_height-3*panel_margin;
    int render_height = panel_height-console_height;

    // Render toolbar
    ImGui::SetNextWindowPos(ImVec2(0, panel_margin), ImGuiCond_Once, (ImVec2){0, 0});
    ImGui::SetNextWindowSize(ImVec2(width, toolbar_height), ImGuiCond_Once);
    ImGui::Begin("Toolbar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::BeginGroup();

    if (!project.ProgramIsRunning()) {
        ImGui::SameLine(0, 10);
        if (ImGui::Button("New", (ImVec2){0,0})) {
        }
        
        ImGui::SameLine(0, 30);
        if (ImGui::Button("Previous", (ImVec2){0,0})) {
            project.PreviousFile();
            LoadCurrentProjectFile();
        }

        ImGui::SameLine(0, 10);
        if (ImGui::Button("Next", (ImVec2){0,0})) {
            project.NextFile();
            LoadCurrentProjectFile();
        }

        ImGui::SameLine(0, 30);
        if (ImGui::Button("Run", (ImVec2){0,0})) {
            project.SaveCurrentFile(m_editor.GetText());
            project.Run();
        }
    }
    else {
        ImGui::SameLine(0, 10);
        if (ImGui::Button("Stop", (ImVec2){0,0})) {
            project.Stop();
        }
    }
    
    if (!project.ProgramIsRunning()) {
        ImGui::SameLine(0, 10);
        if (ImGui::Button("CLS", (ImVec2){0,0})) {
            platform_clear_screen();
            lineBuffer.clear();
        }

        ImGui::SameLine(0, 40);
        if (ImGui::Button("Run on PiTrex", (ImVec2){0,0})) {
            project.SaveCurrentFile(m_editor.GetText());
            project.RunOnPitrex();
        }
    }

    ImGui::EndGroup();
    ImGui::End();

    std::string name = "Editor - PiTrex BASIC";
    ImGui::SetNextWindowPos(ImVec2(panel_margin, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(editorWidth, panel_height), ImGuiCond_Once);
    ImGui::Begin(name.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    m_editor.Render(name.c_str());
    ImGui::End();

    // Render
    name = "Simulator";
    ImGui::SetNextWindowPos(ImVec2(panel_margin+editorWidth, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(panel_width, render_height), ImGuiCond_Once);
    ImGui::Begin(name.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    if (render_flag || !project.ProgramIsRunning()) {
        // Render text buffer
        for (int i = 0; i < textbuffer_get_rows(); i++) {
            char* line = (char *)textbuffer_get_line(i);
            ImGui::Text("%s", line);
        }
        
        // Render line buffer
        for (auto line : lineBuffer) {
            ImVec4 color{1.0,1.0,1.0,1.0};
            app.AddLine(line.x1, line.y1, line.x2, line.y2, color);
        }

        // Hacky
        render_flag = false;
    }

    ImGui::End();

    // Render Console
    ImGui::SetNextWindowPos(ImVec2(panel_margin+editorWidth, panel_top+render_height), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(panel_width, console_height), ImGuiCond_Once);
    ImGui::Begin("Console", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    for (auto line : consoleBuffer) {
        ImGui::Text("%s", line.c_str());
    }
    ImGui::End();
}

void PitrexStudioApp::AddLine(int x1, int y1, int x2, int y2, ImVec4& color) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImVec2 size = window->Size;
    ImVec2 pos = window->Pos;
    pos.y += WINDOW_TITLE_BAR_HEIGHT;
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    x1 += pos.x;
    y1 += pos.y;
    x2 += pos.x;
    y2 += pos.y;
    
    y1 = 410 - y1;
    y2 = 410 - y2;

    drawList->AddLine(ImVec2(x1,y1), ImVec2(x2,y2), ImGui::GetColorU32(color), 1.0f);
}

extern "C" {
    void rbFrame(const float width, const float height) {
        app.Frame(width, height);
    }
    
    const char* rbGetWindowTitle() {
        return "PiTrex Studio 0.1.0";
    }

    ImVec2 rbGetWindowSize() {
        return ImVec2(ScreenWidth, ScreenHeight);
    }

    ImVec4 rbGetBackgroundColor() {
        return ImVec4{0.2f, 0.2f, 0.2f, 1.0};
    }

}

extern "C" {

    void vline_buffer_clear_screen(void) {
        lineBuffer.clear();
    }

    void platform_render_frame() {
        if (std::this_thread::get_id() == RENDER_THREAD_ID) {
            return;
        }

        // very hacky :)
        render_flag = true;
        while (render_flag) {}
    }

    void platform_draw_line(int x1, int y1, int x2, int y2) {
        LINE line = {x1, y1, x2, y2};
        lineBuffer.push_back(line);
    }

    void vectrex_add_color_line(int x1, int y1, int x2, int y2, float r, float g, float b, float a) {
        platform_draw_line(x1, y1, x2, y2);
    }

    void platform_print_console(const char *msg) {
        rb::Logger::debug(msg);
        consoleBuffer.push_back(msg);
    }

    char* platform_get_project_path() { return (char *)project.GetPath().c_str(); }
    char* platform_get_temp_path() { return (char *)project.GetTemporaryPath().c_str(); }

}
