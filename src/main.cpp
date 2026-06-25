#include <iostream>
#include <cctype>
#include "editorState.hpp"
#include "config.hpp"
#include "input.hpp"
#include "output.hpp"
#include "fileOp.hpp"

int main(int argc, char *argv[]) {
    editorConfig ec;
    if (argc >= 2) {
        editorOpen(ec, argv[1]);
    }

    editorSetStatusMsg(ec, "HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-O = open");
    bool Running = true;
    while (Running) { 
        editorRefreshScreen(ec);
        Running = editorProcessKeypress(ec);
    }
    
    return 0;
}
