#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <chrono>
#include <cstdarg>
#include "editorState.hpp"

int CxToRx(editorConfig& EditC, const std::string& line, int cx);

void editorScroll(editorConfig& EditC);

void editorDrawRows(editorConfig& EditC, std::string &s);

void editorDrawStatusBar(editorConfig& EditC, std::string &s);

void editorDrawMessageBar(editorConfig& EditC, std::string& s);

void editorRefreshScreen(editorConfig& EditC);

void editorSetStatusMsg(editorConfig& EditC, const char* fmt, ...);