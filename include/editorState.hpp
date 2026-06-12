#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include "piecetable.hpp"
#include "Terminal.hpp"

class editorConfig {
public:
    size_t cx{0};
    size_t cy{0};
    size_t rx{0};
    size_t screenRows{0};
    size_t screenCols{0};
    size_t numRows{0};
    size_t rowoff{0};
    size_t coloff{0};
    size_t dirty{0};
    
    std::string filename{""};
    std::string statusMsg{""};
    
    pieceTable pt;
    Terminal tl;
    std::chrono::time_point<std::chrono::system_clock> statusmsg_time;

    editorConfig();

    std::string GetLine(size_t idx);

    void cursorUp();
    void cursorDown();
    void cursorLeft();
    void cursorRight();
    void cursorTop();
    void cursorBottom();
    void cursorEdgeRight();
    void cursorEdgeLeft();


    void editorAppendRow();
    void editorInsertNewLine();
    void editorInsertChar(char c);
    void editorDelChar();

};

