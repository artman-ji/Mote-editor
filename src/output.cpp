#include <iostream>
#include <unistd.h>
#include <string>
#include "output.hpp"
#include "config.hpp"

int CxToRx(editorConfig& EditC ,const std::string& line, int cx) {
  int rx = 0;
  for (int i = 0; i < cx; i++) {
    if (line[i] == '\t') {
      rx += (TAB_STOP - 1) - (rx % TAB_STOP);
    }
    rx++;
  }
  if (EditC.cy <= EditC.numRows) {
    rx += std::to_string(EditC.cy + 1).length() + 1;
  }

  return rx;
}

void editorScroll(editorConfig& EditC) {
  EditC.rx = 0;

  if (EditC.cy < EditC.numRows) {
    EditC.rx = (CxToRx(EditC, EditC.GetLine(EditC.cy), EditC.cx));
  }

  if (EditC.cy < EditC.rowoff) {
    EditC.rowoff = EditC.cy;
  }
  if (EditC.cy >= EditC.rowoff + EditC.screenRows) {
    EditC.rowoff = (EditC.cy - EditC.screenRows + 1);
  }

  if (EditC.rx < EditC.coloff) {
    EditC.coloff = EditC.rx;
  }
  if (EditC.rx >= EditC.coloff + EditC.screenCols) {
    EditC.coloff = (EditC.rx - EditC.screenCols + 1);
  }
}

std::string render(const std::string& line) {
  std::string rd = "";
  for (size_t i = 0; i < line.length(); i++) {
    if (line[i] == '\t') {
      rd += " ";
      while (rd.length() % TAB_STOP != 0) {
        rd += " ";
      }
    }
    else {
      rd += line[i];
    }
  }
  return rd;
}

void editorDrawRows(editorConfig& EditC, std::string &s) {
  for (size_t y = 0; y < EditC.screenRows; y++) {
    size_t filerow = y + EditC.rowoff;
    if (filerow >= EditC.numRows) {
      if (EditC.numRows == 0 && y == EditC.screenRows / 3) {
        std::string welcome = "Mote Editor - Version ";
        welcome += MOTEPAD_VERSTION;
        size_t padding = (EditC.screenCols - welcome.length()) / 2;
        if (padding) {
          s += "~";
          padding--;
        }
        while (padding--) {
          s += " ";
        }
        s += welcome;
      }
      else {
        s += "~";
      }
    }
    else {
      std::string l = EditC.GetLine(filerow);
      std::string rd = render(l);
      int len = rd.length() - EditC.coloff;
      if (len < 0) {
        len = 0;
      }
      if (len > static_cast<int>(EditC.screenCols)) {
        len = EditC.screenCols;
      } 
      
      if (static_cast<size_t>(EditC.coloff) < rd.length()) {
        s += std::to_string(filerow + 1) + " ";
        s.append(rd, EditC.coloff, len);
      }

      else if (rd.length() == 0) {
        s += std::to_string(filerow + 1) + " ";
      }
    }
    
    s += "\x1b[K";
    s += "\r\n";
  }
}

void editorDrawStatusBar(editorConfig& EditC, std::string &s) {
  s += "\x1b[7m";
  
  std::string displayFilename = EditC.filename.empty() ? "[No Name]" : EditC.filename.substr(0, 20);
  std::string dirt = EditC.dirty ? "(modified)" : ""; 
  std::string debug = " " + std::to_string(EditC.cx) + " " + std::to_string(EditC.cy) + " " + std::to_string(EditC.pt.getLogicalPosition(EditC.cx, EditC.cy)) + " "+ std::to_string(EditC.pt.TotalLength);
  std::string status = displayFilename + " - " + std::to_string(EditC.numRows) + " lines" + dirt + debug;
  std::string rstatus = std::to_string(EditC.cy + 1) + "/" + std::to_string(EditC.numRows);   

  size_t len = status.length();
  size_t rlen = rstatus.length();
  if (len > static_cast<size_t>(EditC.screenCols)) {
    status = status.substr(0, EditC.screenCols);
    len = EditC.screenCols;
  }

  s.append(status);

  while (static_cast<size_t>(EditC.screenCols) > len) {
    if (EditC.screenCols - len == rlen) {
      s += rstatus;
      break;
    }
    else {
      s += " ";
      len++;
    }
  }
  
  s += "\x1b[m";
  s += "\r\n";
}


void editorDrawMessageBar(editorConfig& EditC, std::string& s) {
  s += "\x1b[K";
  size_t msglen = EditC.statusMsg.length();
  if (msglen > EditC.screenCols) {
    msglen = EditC.screenCols;
  } 
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - EditC.statusmsg_time);
  if (msglen && duration.count() < 5) {
    s.append(EditC.statusMsg);
  }
}

void editorRefreshScreen(editorConfig& EditC) {
  editorScroll(EditC);

  std::string buffer;
  
  buffer += "\x1b[?25l";
  buffer += "\x1b[H";

  editorDrawRows(EditC, buffer);
  editorDrawStatusBar(EditC, buffer);
  editorDrawMessageBar(EditC, buffer);
  
  buffer += "\x1b[" + std::to_string((EditC.cy - EditC.rowoff) + 1) + ";" + std::to_string((EditC.rx - EditC.coloff) + 1) + "H";

  buffer += "\x1b[?25h";

  write(STDOUT_FILENO, buffer.data(), buffer.size());
}

void editorSetStatusMsg(editorConfig& EditC, const char* fmt, ...) {
  va_list args1;
  va_start(args1, fmt);
  
  va_list args2;
  va_copy(args2, args1);
  int len = std::vsnprintf(nullptr, 0, fmt, args2);
  va_end(args2);
  
  if (len > 0) {
      std::vector<char> buffer(len + 1);
      std::vsnprintf(buffer.data(), buffer.size(), fmt, args1);
      EditC.statusMsg = std::string(buffer.data(), len);
  }
  va_end(args1);
  
  EditC.statusmsg_time = std::chrono::system_clock::now();
}