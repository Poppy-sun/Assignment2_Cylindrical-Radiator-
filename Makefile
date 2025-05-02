# Compiler and Flags
CXX = g++
NVCC = nvcc
CXXFLAGS = -O2 -Wall -Iinclude -fopenmp
NVCCFLAGS = -O2 -Iinclude

# Directories
SRC_DIR = src
CPU_DIR = cpu
GPU_DIR = gpu
BUILD_DIR = build

# Target executable
TARGET = radiator_sim

# Source files
CPU_SOURCES = $(wildcard $(CPU_DIR)/*.cc)
SRC_SOURCES = $(wildcard $(SRC_DIR)/*.cc)
GPU_SOURCES = $(wildcard $(GPU_DIR)/*.cu)

# Object files
CPU_OBJECTS = $(patsubst %.cc, $(BUILD_DIR)/%.o, $(notdir $(CPU_SOURCES)))
SRC_OBJECTS = $(patsubst %.cc, $(BUILD_DIR)/%.o, $(notdir $(SRC_SOURCES)))
GPU_OBJECTS = $(patsubst %.cu, $(BUILD_DIR)/%.o, $(notdir $(GPU_SOURCES)))

OBJECTS = $(CPU_OBJECTS) $(SRC_OBJECTS) $(GPU_OBJECTS)

# Default Rule
all: $(BUILD_DIR) $(TARGET)

# Linking Rule
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) -lcudart -L/usr/local/cuda/lib64

# Build rules for each source type
$(BUILD_DIR)/%.o: $(CPU_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(GPU_DIR)/%.cu
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
