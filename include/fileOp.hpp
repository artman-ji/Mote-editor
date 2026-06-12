#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "editorState.hpp"
#include "output.hpp"
#include "input.hpp"

void editorOpen(editorConfig& EditC, const std::string& filename);

void editorSave(editorConfig& EditC);

