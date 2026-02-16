CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -fPIC
LDFLAGS = -lm

# 目录
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# 源文件
CORE_SOURCES = $(wildcard $(SRC_DIR)/core/*.c)
LEXER_SOURCES = $(wildcard $(SRC_DIR)/lexer/*.c)
PARSER_SOURCES = $(wildcard $(SRC_DIR)/parser/*.c)
VM_SOURCES = $(wildcard $(SRC_DIR)/vm/*.c)
JIT_SOURCES = $(wildcard $(SRC_DIR)/jit/*.c)
API_SOURCES = $(wildcard $(SRC_DIR)/api/*.c)
STDLIB_SOURCES = $(wildcard $(SRC_DIR)/stdlib/*.c)

ALL_SOURCES = $(CORE_SOURCES) $(LEXER_SOURCES) $(PARSER_SOURCES) $(VM_SOURCES) $(JIT_SOURCES) $(API_SOURCES) $(STDLIB_SOURCES)
OBJECTS = $(ALL_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# 目标
TARGET = libminiscript.a
SHARED_TARGET = libminiscript.so
INTERPRETER = miniscript

.PHONY: all clean dirs

all: dirs $(TARGET) $(SHARED_TARGET) $(INTERPRETER)

dirs:
	@mkdir -p $(BUILD_DIR)/core $(BUILD_DIR)/lexer $(BUILD_DIR)/parser $(BUILD_DIR)/vm $(BUILD_DIR)/jit $(BUILD_DIR)/api $(BUILD_DIR)/stdlib

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TARGET): $(OBJECTS)
	ar rcs $@ $^

$(SHARED_TARGET): $(OBJECTS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

$(INTERPRETER): $(TARGET) src/main.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ src/main.c -L. -lminiscript $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(SHARED_TARGET) $(INTERPRETER)

install: all
	cp $(TARGET) /usr/local/lib/
	cp $(SHARED_TARGET) /usr/local/lib/
	cp -r $(INCLUDE_DIR)/* /usr/local/include/
	cp $(INTERPRETER) /usr/local/bin/