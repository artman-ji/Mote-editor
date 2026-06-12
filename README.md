# Mote Text Editor

Mote is a lightweight, terminal-based text editor written in modern C++. this has been a hobby project that i have been working on, and it is based on the **kilo** text editor,written by Salvatore Sanfilippo (antirez). 

At its core, Mote utilizes a **Piece Table** data structure written by me, related Documentation will soon be added. 

## Project Structure

```text
mote/
├── CMakeLists.txt (or Makefile)  # Build system configuration
├── README.md                     # Project description and instructions
├── include/                      # Public header files (.hpp)
│   └── mote/                     
│       ├── editorState.hpp       # Global editor configuration and cursor state
│       ├── fileOp.hpp            # File I/O operations (Open, Save)
│       ├── input.hpp             # Keypress processing and prompt handling
│       ├── output.hpp            # Screen rendering and ANSI drawing logic
│       ├── pieceTable.hpp        # Core text management data structure
│       └── Terminal.hpp          # Raw mode setup and terminal dimension queries
├── src/                          # Source files (.cpp)
│   ├── main.cpp                  # Editor entry point
│   ├── editorState.cpp
│   ├── fileOp.cpp
│   ├── input.cpp
│   ├── output.cpp
│   ├── pieceTable.cpp
│   └── Terminal.cpp
├── tests/                        # Fuzz tests and unit tests
│   └── pieceTable_test.cpp       # Tests for text buffer integrity
├── build/                        # Compiled object files and executables
└── docs/                         # Additional project documentation(to be added)
```
## Core Components

* **`Terminal`:** Safely transitions the console into raw mode and restores it upon exit. It handles reading raw bytes and querying the window size, and it uses the VT100 escape sequences. 
* **`pieceTable`:** The mathematical heart of the editor. it tracks text edits as a series of "pieces" pointing to original or appended text buffers, allowing operations like $pos = x + y$ to be calculated virtually.
* **`editorState` (`editorConfig`):** Holds the global state, including window dimensions, cursor coordinates ($cx, cy$), scroll offsets, and the dirty flag for unsaved changes.
* **`output` / `input`:** Manage the translation between the user's keystrokes (like arrows or `Ctrl+S`) and visual updates on the screen.

## Building and Running

- a Unix environment(Linux, macOS, or WSL on Windows).
- Ensure you have a modern C++ compiler (supporting C++11) installed. 
- Ensure you have cmake installed.

### Using Make

1. Compile the project: 
```bash
make
```
2. Run the editor without input file: 
```bash
./build/mote
``` 
3. with input file:
```bash
./build/mote filename.txt
```
4. cleaning the build directory:
```bash
make clean
```

## keys:

```text
CTRL-S: Save
CTRL-Q: Quit
```

## Testing

right now, it only includes a `temptest.cpp` file meant to test the piecetable but it has not been configured to run with the full project, it uses a fuzz testing method to test all things related to edit operations within the piecetable.


## Future
There are several things i wish to add to this project later on:
1. `undo() / redo()` functions.

2. line numbers

3. syntax highlighting

4. search function

5. copy and paste

6. and many more.

## Acknowledgements

This project is deeply inspired by and based upon the **Kilo** text editor, originally written in C by Salvatore Sanfilippo (antirez). 

While Mote has been completely refactored into an object-oriented C++11 architecture utilizing a Piece Table, the core terminal handling, ANSI escape sequence logic, and general editor lifecycle heavily owe their design to the elegant simplicity of Kilo.

## License

Mote is open-source software and is available under the [MIT License](LICENSE). 

Because Mote is a derivative work of Kilo, it respects the original **BSD 2-Clause License** of the Kilo project. 
