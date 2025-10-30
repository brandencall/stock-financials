# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -Iinclude -Iexternal/rapidfuzz-cpp/include -MMD -MP 
LIBS = -lncurses -lcurl

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/tui

# Sources
SRCS = $(shell find $(SRC_DIR) -name '*.cpp') 
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS)) 
DEPS = $(OBJECTS:.o=.d)

# Targets
all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ $(LIBS) -o $@ 

# Compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Directories
$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
.PHONY: all clean
