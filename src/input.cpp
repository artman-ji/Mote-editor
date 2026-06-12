#include <iostream>
#include <unistd.h>
#include "input.hpp"

std::string editorPrompt(editorConfig& EditC, const std::string& prompt) {
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
    if (c == '\x1b') {
      editorSetStatusMsg(EditC, "");
      return "";
    }
    if (c == '\r') {
      if (!userInput.empty()) {
        editorSetStatusMsg(EditC, "");
        return userInput;
      }
    } 
    else if (!iscntrl(c) && c < 128) {
      userInput += static_cast<char>(c);
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

    case CTRL_KEY('l'):
    case '\x1b':
      break;

    default:
      EditC.editorInsertChar(c);
      break;
  }
  quit = QUIT_TIMES;
}