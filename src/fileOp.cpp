#include "fileOp.hpp"

void editorOpen(editorConfig& EditC, const std::string& filename) {
    if (filename.empty()) {
        EditC.filename = editorPrompt(EditC, "Open file: %s (ESC to cancel)", nullptr);
        if (EditC.filename.empty()) {
            editorSetStatusMsg(EditC, "Open aborted");
            return;
        }        
    }
    else {
        EditC.filename = filename;
    }
    std::ifstream fp(EditC.filename);
    if (!fp) {
        std::cout << "error!" << std::endl;
    }

    std::string file;
    std::string line;
    while (std::getline(fp, line)) {
        file += line;
        file += "\n";
        EditC.numRows++;
    }

    EditC.pt.init(file);
    EditC.dirty = 0;
}


void editorSave(editorConfig& EditC) {
    if (EditC.filename.empty()) {
        EditC.filename = editorPrompt(EditC, "Save as: %s (ESC to cancel)", nullptr);
        if (EditC.filename.empty()) {
            editorSetStatusMsg(EditC, "Save aborted");
            return;
        }
    }

    std::ofstream outFile(EditC.filename, std::ios::out | std::ios::trunc);

    if (outFile.is_open()) {
        std::string content = EditC.pt.DisplayText();
        outFile << content;
        
        if (outFile.good()) {
            EditC.dirty = 0;
            editorSetStatusMsg(EditC, "Saved %zu bytes to disk", content.length());
        } 
        else {
            editorSetStatusMsg(EditC, "Can't save! I/O error occurred.");
        }
    } 
    else {
        editorSetStatusMsg(EditC, "Can't save! Could not open file.");
    }
}
