# Detect platform
UNAME_S := $(shell uname -s)

# Default Compiler and Flags
ifeq ($(UNAME_S), Darwin)
    CXX = /opt/homebrew/opt/llvm/bin/clang++
    CXXFLAGS = -std=c++17 -O3 -ffast-math -fopenmp
else
    CXX = g++
    CXXFLAGS = -std=c++17 -O3 -march=native -fopenmp -pthread -Wall -Wextra -Wshadow -w
endif

# Directories
BIN_DIR = ../bin
LIB_DIR = ../lib/stockfish_nnue_probe

# Source Files
SRC_COMMON = search.cpp evaluation.cpp
SRC_DONBOT = donbot.cpp
SRC_DEBUG = debug.cpp

# Output Binaries
BIN_DONBOT = $(BIN_DIR)/donbot
BIN_DEBUG = $(BIN_DIR)/debug

# Include Directories
INCLUDE_DIR = -I include/ -I $(LIB_DIR)

# Build Targets
all: donbot debug

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

donbot: $(SRC_DONBOT) $(SRC_COMMON) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(BIN_DONBOT) $(SRC_DONBOT) $(SRC_COMMON)

debug: $(SRC_DEBUG) $(SRC_COMMON) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) -o $(BIN_DEBUG) $(SRC_DEBUG) $(SRC_COMMON)

# Clean
clean:
	rm -rf $(BIN_DIR)

.PHONY: all donbot debug clean
