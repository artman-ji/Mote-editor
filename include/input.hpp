#pragma once
#include <iostream>
#include <unistd.h>
#include <functional>
#include "config.hpp"
#include "editorState.hpp"
#include "fileOp.hpp"

int RxToCx(const std::string& line, int rx);

std::string editorPrompt(editorConfig& EditC, const std::string& prompt, std::function<void(editorConfig&, std::string, int)> callback);

void editorFindCallback(editorConfig& EditC, std::string query, int key);

void editorFind(editorConfig& EditC);

void editorMoveCursor(editorConfig& EditC, int k);

void editorProcessKeypress(editorConfig& EditC);
