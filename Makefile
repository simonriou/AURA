BREW_PREFIX = $(shell brew --prefix)

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O3 -I./include -I$(BREW_PREFIX)/include
LDFLAGS = -L$(BREW_PREFIX)/lib -lsndfile -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/aura

# Find all .c files in src/ and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.c')

# Generate corresponding .o file names in the build/ directory
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Default target
all: directories $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile the .c files into .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
directories:
	@mkdir -p $(BUILD_DIR)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all directories clean