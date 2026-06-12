# specifying Compiler
CXX = g++

# flag settings for compiler
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g -Iinclude

# directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# final executable name
TARGET = $(BUILD_DIR)/moted

# finding all .cpp files in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# mapping every .cpp file to a .o object file in the build directory
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Default target
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build successful! Run with ./$(TARGET)"

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

.PHONY: all clean
