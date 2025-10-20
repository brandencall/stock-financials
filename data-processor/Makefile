# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -Iinclude -MMD -MP -I../database/include
LIB_DIR = ../database/lib
LIBS = -lcurl -lstockdatabase -lsqlite3 -lcrypto

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Binaries
TARGET = $(BIN_DIR)/data_proc

# Sources
SRCS = $(shell find $(SRC_DIR) -name '*.cpp') 

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

DEPS = $(OBJECTS:.o=.d)

# Targets
all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ -L$(LIB_DIR) $(LIBS) -o $@ 

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
