#include <iostream>
#include <unistd.h>
#include "input.hpp"

int RxToCx(std::string& line, int rx) {
    int curRx = 0;
    size_t cx;
    for (cx = 0; cx < line.length(); cx++) {
        if (line[cx] == '\t') {
            curRx += (TAB_STOP - 1) - (curRx % TAB_STOP);
        }
        curRx++;
        if (curRx > rx) {
            return cx;
        } 
    }
    return cx;
}

void editorFindCallback(editorConfig& EditC, std::string query, int key) {
    static int direction = 1;
    static int lastMatch = -1;
    if (key == '\r' || key == '\x1b') {
        lastMatch = -1;
        direction = 1;
        return;
    }
    else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
        direction = 1;
    }
    else if (key == ARROW_LEFT || key == ARROW_UP) {
        direction = -1;
    }
    else {
        lastMatch = -1;
        direction = 1;
    }
  
    if (lastMatch == -1) {
        direction = 1;
    }

    int current = lastMatch;
    size_t i;
    for (i = 0; i < EditC.numRows; i++) {
        current += direction;

        if (current == -1) {
            current = static_cast<int>(EditC.numRows - 1);
        }
        else if (current == static_cast<int>(EditC.numRows)) {
            current = 0;
        }

        std::string line = EditC.GetLine(current);
        std::size_t pos = line.find(query);
        if (pos != std::string::npos) {
            lastMatch = current;
            EditC.cy = current;
            EditC.cx = RxToCx(line, pos);
            EditC.rowoff = EditC.numRows;
            break;
        }
    }
}

void editorFind(editorConfig& EditC) {
    size_t savedCx = EditC.cx;
    size_t saved_cy = EditC.cy;
    size_t savedColoff = EditC.coloff;
    size_t savedRowoff = EditC.rowoff;
    
    std::string query = editorPrompt(EditC, "Search: %s (ESC to cancel)", editorFindCallback);
    
    if (query.empty()) {
        editorSetStatusMsg(EditC, "Find Aborted");
        
        EditC.cx = savedCx;
        EditC.cy = saved_cy;
        EditC.coloff = savedColoff;
        EditC.rowoff = savedRowoff;
        
      return;
    }
}

std::string editorPrompt(editorConfig& EditC, const std::string& prompt, std::function<void(editorConfig&, std::string, int)> callback) {
  std::string userInput = "";
  while (true) {
    std::string statusMsg = prompt;
    size_t pos = statusMsg.find("%s");
    if (pos != std::string::npos) {
      statusMsg.replace(pos, 2, userInput);
    }

    editorSetStatusMsg(EditC, userInput.c_str());
    editorRefreshScreen(EditC);
    int c = EditC.tl.editorReadKey();
    if (c == DELETE_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      if (!userInput.empty()) {
        userInput.pop_back();
      } 
    }
    else if (c == '\x1b') {
      editorSetStatusMsg(EditC, "");
      if (callback) {
        callback(EditC, userInput, c);
      }
      return "";
    }
    else if (c == '\r') {
      if (!userInput.empty()) {
        editorSetStatusMsg(EditC, "");
        if (callback) {
          callback(EditC, userInput, c);
        }
        return userInput;
      }
    } 
    else if (!iscntrl(c) && c < 128) {
      userInput += static_cast<char>(c);
    }
    if (callback) {
      callback(EditC, userInput, c);
    }
  } 
}

void editorMoveCursor(editorConfig& EditC, int k) {
  switch (k) {
  case ARROW_LEFT:
    EditC.cursorLeft();
    break;
  case ARROW_RIGHT:
    EditC.cursorRight();
    break;
  case ARROW_DOWN:
    EditC.cursorDown();
    break;
  case ARROW_UP:
    EditC.cursorUp();
    break;
  case PAGE_UP:
    EditC.cursorTop();
    break;
  case PAGE_DOWN:
    EditC.cursorBottom();
    break;
  case HOME_KEY:
    EditC.cursorEdgeLeft();
    break;
  case END_KEY:
    EditC.cursorEdgeRight();
    break;
  }

  std::string row = (EditC.cy >= EditC.numRows) ? "" : EditC.GetLine(EditC.cy);
  size_t l = row.length();
  if (EditC.cx > l) {
    EditC.cx = l;
  }
}

void editorProcessKeypress(editorConfig& EditC) {
  static int quit = QUIT_TIMES;

  int c = EditC.tl.editorReadKey();
  switch (c) {
    case '\r':
      EditC.editorInsertNewLine();
      break;
    case CTRL_KEY('q'):
      if (EditC.dirty && quit > 0) {
        editorSetStatusMsg(EditC, "WARNING!!! File has unsaved changes. "
          "Press Ctrl-Q %d more times to quit.", quit);
        quit--;
        return;
      }
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DELETE_KEY:
      if (c == DELETE_KEY) {
        editorMoveCursor(EditC, ARROW_RIGHT);
      }
      EditC.editorDelChar();
      break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
    case PAGE_UP:
    case PAGE_DOWN:
    case HOME_KEY:
    case END_KEY:
      editorMoveCursor(EditC, c);
      break;

    case CTRL_KEY('s'):
      editorSave(EditC);
      break;

    case CTRL_KEY('f'):
      editorFind(EditC);
      break;

    case CTRL_KEY('l'):
    case '\x1b':
      break;

    default:
      EditC.editorInsertChar(c);
      break;
  }
  quit = QUIT_TIMES;
}