#pragma once
#include <iostream>
#include <unistd.h>
#include "config.hpp"
#include "editorState.hpp"
#include "fileOp.hpp"

std::string editorPrompt(editorConfig& EditC, const std::string& prompt);

void editorMoveCursor(editorConfig& EditC, int k);

void editorProcessKeypress(editorConfig& EditC);
