#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>

enum BufferType {
    original,
    append
};

struct piece {
    BufferType source;
    size_t offset;
    size_t length;
};


class pieceTable {
private:
    std::string originalBuffer;
    std::string appendBuffer;
    std::vector<piece> pieces;
    std::vector<size_t> lineStarts;
public:
    size_t TotalLength;
    pieceTable();
    pieceTable(const std::string& input);
    ~pieceTable();

    void init(const std::string& s);
    
    bool Insert(char c, size_t pos);
    bool Delete(size_t pos);
    void computeLineStarts();

    std::pair<size_t, size_t> searchForIns(size_t pos) const;
    std::pair<size_t, size_t> searchForDel(size_t pos) const;
    void DisplayTable() const;
    std::string DisplayText() const;
    size_t rowLength(size_t idx) const;
    std::string getSubstring(size_t pos, size_t length) const;
    std::string getLine(size_t lineNum) const;
    size_t getLogicalPosition(size_t cx, size_t cy) const;
};

