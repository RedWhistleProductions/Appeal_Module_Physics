# Compiler
CXX ?= g++

# Compiler flags
CXXFLAGS ?= -g -Wall
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?=

# Directories
SRC_DIR = ./src
OBJ_DIR = ./OBJ
BIN_DIR = .

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Plugin output
ifeq ($(OS),Windows_NT)
	LIB_EXT = .dll
	SHARED_FLAGS = -shared
else
	LIB_EXT = .so
	SHARED_FLAGS = -shared
	CXXFLAGS += -fPIC
endif

TARGET = $(BIN_DIR)/Test$(LIB_EXT)

# Default target
all: $(TARGET)

# Link object files to create the plugin library
$(TARGET): $(OBJ_FILES)
	$(CXX) $(SHARED_FLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(wildcard src/*.h)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
