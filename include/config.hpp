#pragma once

#define MOTEPAD_VERSTION "0.3.0" 

#define TAB_STOP 8

#define QUIT_TIMES 3

#define LINENUMBER true // value for Line Numbering

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT, 
    ARROW_UP,
    ARROW_DOWN,
    DELETE_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN    
};