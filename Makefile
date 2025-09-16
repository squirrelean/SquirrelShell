CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude -Ibuiltins -Idisplay

SRC_DIRS = src builtins display
BUILD_DIR = build
TARGET = squirrelshell

SRC = $(shell find $(SRC_DIRS) -name '*.c')
OBJ = $(patsubst %,$(BUILD_DIR)/%,$(SRC:.c=.o))

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
