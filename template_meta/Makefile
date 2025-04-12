# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

# Project structure
SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/template_meta

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Link target
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Install demo (optional)
install: $(TARGET)
	@echo "Installing to /usr/local/bin"
	@cp $(TARGET) /usr/local/bin