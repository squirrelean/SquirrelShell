CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude -Ibuiltins

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
BUILTINS_DIR = builtins

TARGET = squirrelshell
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(BUILTINS_DIR)/*.c)
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRC)))
DEPS = $(wildcard $(INCLUDE_DIR)/*.h) $(wildcard $(BUILTINS_DIR)/*.h)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(BUILTINS_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
