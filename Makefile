CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

TARGET = squirrelshell
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEPS = $(wildcard $(INCLUDE_DIR)/*.h)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
