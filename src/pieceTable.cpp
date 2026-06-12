#include "piecetable.hpp"

pieceTable::pieceTable() : TotalLength(0) {
    
    lineStarts.clear();
    lineStarts.push_back(0);
    
    piece initialPiece;
    initialPiece.source = original;
    initialPiece.offset = 0;
    initialPiece.length = 0;
    pieces.push_back(initialPiece);
}

pieceTable::pieceTable(const std::string& input) : originalBuffer(input), TotalLength(input.length()) {
    piece orig;
    orig.source = original;
    orig.offset = 0;
    orig.length = input.length();
    pieces.push_back(orig);
    
    computeLineStarts();
}

pieceTable::~pieceTable() = default;


void pieceTable::init(const std::string& s) {
    originalBuffer = s;
    pieces.clear();

    piece orig;
    orig.source = original;
    orig.offset = 0;
    orig.length = s.length();
    pieces.push_back(orig);

    computeLineStarts();
    TotalLength = s.length();
}

std::pair<size_t, size_t> pieceTable::searchForIns(size_t pos) const {
    size_t logPos = 0;
    for (size_t i = 0; i < pieces.size(); i++) {
        if (pos <= logPos + pieces[i].length) {
            return {i, logPos};
        }
        logPos += pieces[i].length;
    }
    if (pos == logPos && !pieces.empty()) {
        return {pieces.size() - 1, logPos - pieces.back().length}; 
    }
    return {std::string::npos, std::string::npos};
}

std::pair<size_t, size_t> pieceTable::searchForDel(size_t pos) const {
    size_t logPos = 0;
    for (size_t i = 0; i < pieces.size(); i++) {
        if (pos < logPos + pieces[i].length) {
            return {i, logPos};
        }
        logPos += pieces[i].length;
    }
    if (pos == logPos && !pieces.empty()) {
        return {pieces.size() - 1, logPos - pieces.back().length}; 
    }
    return {std::string::npos, std::string::npos};
}


bool pieceTable::Insert(char c, size_t pos) {
    std::pair<size_t, size_t> p = searchForIns(pos);
    size_t idx = p.first;
    size_t logStart = p.second;

    if (idx == std::string::npos) {
        return false;
    }

    appendBuffer += c;
    
    if (logStart == pos) {
        piece newAdd;
        newAdd.source = append;
        newAdd.offset = appendBuffer.length() - 1;
        newAdd.length = 1;

        pieces.insert(pieces.begin() + idx, newAdd);
    }

    else if (logStart + pieces[idx].length == pos) {
        if (pieces[idx].source == append && pieces[idx].offset + pieces[idx].length == appendBuffer.length() - 1) {
            pieces[idx].length++;
        }
        else {
            piece newAdd;
            newAdd.source = append;
            newAdd.offset = appendBuffer.length() - 1;
            newAdd.length = 1;
            pieces.insert(pieces.begin() + idx + 1, newAdd);
        }
    }

    else {
        piece orig1;
        orig1.source = pieces[idx].source;
        orig1.offset = pieces[idx].offset;
        orig1.length = pos - logStart;

        piece orig2;
        orig2.source = append;
        orig2.offset = appendBuffer.length() - 1;
        orig2.length = 1;

        piece orig3;
        orig3.source = pieces[idx].source;
        orig3.offset = pieces[idx].offset + pos - logStart;
        orig3.length = pieces[idx].length - pos + logStart;


        pieces[idx] = orig1;
        pieces.insert(pieces.begin() + idx + 1, orig2);
        pieces.insert(pieces.begin() + idx + 2, orig3);
    }

    computeLineStarts();
    return true;
}

bool pieceTable::Delete(size_t pos) {
    std::pair<size_t, size_t> p = searchForDel(pos);
    size_t idx = p.first;
    size_t logStart = p.second; 
    if (idx == std::string::npos) {
        return false;
    }

    
    if (logStart == pos) {
        pieces[idx].offset++;
        pieces[idx].length--;
        if (pieces[idx].length == 0 && pieces.size() != 1) {
            pieces.erase(pieces.begin() + idx);
        }
    }

    else if (logStart + pieces[idx].length - 1 == pos) {
        pieces[idx].length--;
        if (pieces[idx].length == 0 && pieces.size() != 1) {
            pieces.erase(pieces.begin() + idx);
        }
    }
    
    else {
        piece orig1;
        orig1.source = pieces[idx].source;
        orig1.offset = pieces[idx].offset;
        orig1.length = pos - logStart;

        piece orig2;
        orig2.source = pieces[idx].source;
        orig2.offset = pieces[idx].offset + pos + 1 - logStart;
        orig2.length = pieces[idx].length - pos - 1 + logStart;

        pieces[idx] = orig1;
        pieces.insert(pieces.begin() + idx + 1, orig2);
    }

    computeLineStarts();
    return true;
}

void pieceTable::DisplayTable() const {
    for (const auto& piece : pieces) {
        std::string O = (piece.source == original) ? "original" : "add";
        O += " | " + std::to_string(piece.offset) + " | " + std::to_string(piece.length);
        std::cout << O << '\n';
    }
}

std::string pieceTable::DisplayText() const {
    std::string finalText = "";
    finalText.reserve(TotalLength);
    
    for (const auto& piece : pieces) {
        if (piece.length == 0) {
            continue;
        }
        if (piece.source == original) {
            finalText.append(originalBuffer, piece.offset, piece.length);
        } else if (piece.source == append) {
            finalText.append(appendBuffer, piece.offset, piece.length);
        }
    }

    return finalText;
}

void pieceTable::computeLineStarts() {
    lineStarts.clear();
    lineStarts.push_back(0); 

    std::string fullText = DisplayText();
    TotalLength = fullText.length();
    
    for (size_t i = 0; i < fullText.length(); ++i) {
        if (fullText[i] == '\n') {
            lineStarts.push_back(i + 1);
        }
    }
}

size_t pieceTable::rowLength(size_t idx) const {
    if (idx == 0 || idx >= lineStarts.size()) {
        return 0;
    }
    return lineStarts[idx] - lineStarts[idx - 1];
}

std::string pieceTable::getSubstring(size_t pos, size_t length) const {
    std::string result;
    
    if (length == 0 || pieces.empty()) {
        return result;
    }
    
    result.reserve(length);

    size_t charsNeeded = length;
    size_t currentPos = pos;

    std::pair<size_t, size_t> p = searchForDel(currentPos);
    size_t pieceIdx = p.first;
    size_t currentLogStart = p.second;

    while (charsNeeded > 0 && pieceIdx < pieces.size()) {
        const piece& currentPiece = pieces[pieceIdx];

        size_t offsetInPiece = currentPos - currentLogStart;
        size_t availableInPiece = currentPiece.length - offsetInPiece;
        size_t charsToTake = std::min(availableInPiece, charsNeeded);

        const std::string& sourceBuf = (currentPiece.source) ? appendBuffer : originalBuffer;
        result.append(sourceBuf, currentPiece.offset + offsetInPiece, charsToTake);

        charsNeeded -= charsToTake;
        currentPos += charsToTake;
        currentLogStart += currentPiece.length; 
        pieceIdx++;
    }

    return result; 
}

std::string pieceTable::getLine(size_t lineNum) const {
    if (lineNum >= lineStarts.size()) {
        return ""; 
    }

    size_t pos = lineStarts[lineNum];
    size_t length = 0;

    if (lineNum + 1 < lineStarts.size()) {
        length = lineStarts[lineNum + 1] - pos;
    } else {
        length = TotalLength - pos;
    }

    std::string lineStr = getSubstring(pos, length);

    if (!lineStr.empty() && lineStr.back() == '\n') {
        lineStr.pop_back();
        if (!lineStr.empty() && lineStr.back() == '\r') {
            lineStr.pop_back();
        }
    }

    return lineStr;
}

size_t pieceTable::getLogicalPosition(size_t cx, size_t cy) const {
    if (lineStarts.empty() || cy >= lineStarts.size()) {
        return TotalLength;
    }

    size_t startOfLine = lineStarts[cy];
    size_t nextLineStart = TotalLength;
    
    if (cy + 1 < lineStarts.size()) {
        nextLineStart = lineStarts[cy + 1];
    }

    size_t lenOfLine = nextLineStart - startOfLine; 

    if (nextLineStart < TotalLength) {
        lenOfLine--;
    }

    size_t safeCx = std::min(cx, lenOfLine);
    return startOfLine + safeCx;
}

