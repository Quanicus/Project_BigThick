# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -march=native -O3 -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++20 -flto=auto

# Libraries
LIBS := -luSockets -lz

# Source and target
SRC := UpgradeSync.cpp
TARGET := UpgradeSync

# Default target
all: $(TARGET)

# Compilation rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

# Clean rule
clean:
	rm -f $(TARGET)
