#pragma once
#include <termios.h>
#include <string>
#include "piecetable.hpp"


class Terminal {
private:
    struct termios orig_termios;
    
public:
    Terminal();
    ~Terminal();
    
    void die(const std::string& s);
    int editorReadKey();
    
    int getCursorPosition(int* rows, int* cols);
    int getWindowSize(int* rows, int* cols);
};
