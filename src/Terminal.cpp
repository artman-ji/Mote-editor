#include "Terminal.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <sys/ioctl.h>
#include "config.hpp"

Terminal::Terminal() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        die("tcgetattr");
    }
    
    struct termios raw = orig_termios;
  
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
  
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        die("tcsetattr");
    }
}

Terminal::~Terminal() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
    }
}

void Terminal::die(const std::string& s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s.c_str());
    exit(1);
}

int Terminal::editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }

    if (c == '\x1b') {
        char buf[3];
        if (read(STDIN_FILENO, &buf[0], 1) != 1) {
            return '\x1b';
        }
        if (read(STDIN_FILENO, &buf[1], 1) != 1) {
            return '\x1b';
        }
        if (buf[0] == '[') {
            if (buf[1] <= '9' && buf[1] >= '0') {
                if (read(STDIN_FILENO, &buf[2], 1) != 1) {
                    return '\x1b';
                }
                if (buf[2] == '~') {
                    switch (buf[1]) {
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '1':
                    case '7':
                        return HOME_KEY;
                    case '4':
                    case '8':
                        return END_KEY;
                    case '3':
                        return DELETE_KEY;
                    }
                }
            }
            else {
                switch (buf[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
                }   
            }
        }
        else if (buf[0] == 'O') {
            switch (buf[1]) {
            case 'H':
                return HOME_KEY;
            case 'F':
                return END_KEY;
            }
        }
        return '\x1b';
    }
    return c;    
}

int Terminal::getCursorPosition(int *rows, int *cols) {
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
        return -1;
    } 

    std::string buf;
    char c;

    while (buf.length() < 32) {
        if (read(STDIN_FILENO, &c, 1) != 1) { 
            break;
        }
        
        buf.push_back(c);
        
        if (c == 'R') {
            break;
        }
    }

    if (buf.length() < 2 || buf[0] != '\x1b' || buf[1] != '[') {
        return -1;
    }
    
    if (std::sscanf(buf.c_str() + 2, "%d;%d", rows, cols) != 2) {
        return -1;
    }
    
    return 0;
}

int Terminal::getWindowSize(int* rows, int* cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
            return -1;
        }
        return getCursorPosition(rows, cols);
    } 
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}