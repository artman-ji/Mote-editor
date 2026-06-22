#include "editorState.hpp"

editorConfig::editorConfig() {
    int rows, cols;
    if (tl.getWindowSize(&rows, &cols) == -1) {
        tl.die("getWindowSize");
    }
    screenRows = static_cast<size_t>(rows) - 2;
    screenCols = static_cast<size_t>(cols);
}

std::string editorConfig::GetLine(size_t idx) {
    return pt.getLine(idx);
}


void editorConfig::cursorUp() {
    if (cy > 0) {
        cy--;
    }
}

void editorConfig::cursorDown() {
    if (cy < numRows) {
        cy++;
    }
}

void editorConfig::cursorLeft() {
    if (cx > 0) {
        cx--;
    }
    else if (cy > 0) {
        cy--;
        cx = GetLine(cy).length();
    }
}

void editorConfig::cursorRight() {
    if (cy < numRows && cx < GetLine(cy).length()) {
        cx++;
    }
    else if (cy < numRows && cx == GetLine(cy).length()) {
        cy++;
        cx = 0;
    }
}

void editorConfig::cursorTop() {
    cy = rowoff;
    for (size_t i = 0; i < screenRows; i++) {
        cursorUp();
    }
}

void editorConfig::cursorBottom() {
    cy = rowoff + screenRows - 1;
    if (cy > numRows) {
        cy = numRows;
    }
    for (size_t i = 0; i < screenRows; i++) {
        cursorDown();
    }
}

void editorConfig::cursorEdgeLeft() {
    cx = 0;
}

void editorConfig::cursorEdgeRight() {
    if (cy < numRows) {
        cx = GetLine(cy).length();
    }
}

void editorConfig::editorAppendRow() {
    size_t pos = 0;
    if (cy >= numRows && numRows > 0) {
        pos = pt.getLogicalPosition(GetLine(numRows - 1).length(), numRows - 1);
    }
    else {
        pos = pt.getLogicalPosition(cx, cy);
    }
    
    pt.Insert('\n', pos);
    dirty++;
    numRows++;  
}


void editorConfig::editorInsertNewLine() {
    editorAppendRow();
    cy++;
    cx = 0;
}

void editorConfig::editorInsertChar(char c) {
    if (cy == numRows) {
        editorAppendRow();
    }
    size_t pos = pt.getLogicalPosition(cx, cy);
    pt.Insert(c, pos);
    cx++;
    dirty++;
}

void editorConfig::editorDelChar() {
    if (cy == numRows) {
        return;
    }
    if (cx == 0 && cy == 0) {
        return;
    }
    size_t pos = pt.getLogicalPosition(cx, cy);
    if (cx > 0) {
        pt.Delete(pos - 1);
        cx--;
    }
    else {
        pt.Delete(pos - 1);
        cx = pt.getLine(cy - 1).length();
        cy--;
        numRows--;
    }
    dirty++;
}