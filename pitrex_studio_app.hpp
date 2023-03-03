
#pragma once

#include "rbbase/RBTimer.hpp"
#include "texteditor/TextEditor.h"

class PitrexStudioApp {
public:
    PitrexStudioApp();
    
public:
    void Frame(const float width, const float height);
    void AddLine(int x1, int y1, int x2, int y2, ImVec4& color);

private:
    void Initialise();
    void LoadCurrentProjectFile();

private:
    bool m_calledFirstTime;
    int m_width;
    int m_height;
    TextEditor m_editor;
    rb::Timer m_timer;
};
