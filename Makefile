CXX = g++
CXXFLAGS = -O2 -Wall -Iinclude

# Project structure
SRC_DIR = src
CPU_DIR = cpu
INCLUDE_DIR = include
BUILD_DIR = build

# Output binary
TARGET = radiator_sim

# Source files
CPU_SOURCES = $(wildcard $(CPU_DIR)/*.cc)
SRC_SOURCES = $(wildcard $(SRC_DIR)/*.cc)

# Object files
CPU_OBJECTS = $(patsubst $(CPU_DIR)/%.cc, $(BUILD_DIR)/%.o, $(CPU_SOURCES))
SRC_OBJECTS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRC_SOURCES))
OBJECTS = $(CPU_OBJECTS) $(SRC_OBJECTS)

# Default rule
all: $(BUILD_DIR) $(TARGET)

# Link objects into binary
$(TARGET): $(OBJECTS)
        $(CXX) $(CXXFLAGS) -o $@ $^

# Compile CPU sources
$(BUILD_DIR)/%.o: $(CPU_DIR)/%.cc
        $(CXX) $(CXXFLAGS) -c $< -o $@

# Compile SRC sources
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
        $(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory
$(BUILD_DIR):
        mkdir -p $(BUILD_DIR)

# Clean
clean:
        rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
