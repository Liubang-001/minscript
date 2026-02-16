CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -fPIC
LDFLAGS = -lm

# Windows特定设置
ifeq ($(OS),Windows_NT)
    TARGET = miniscript.exe
    SHARED_TARGET = miniscript.dll
    INTERPRETER = miniscript.exe
    RM = del /Q
    MKDIR = mkdir
    CP = copy
else
    TARGET = libminiscript.a
    SHARED_TARGET = libminiscript.so
    INTERPRETER = miniscript
    RM = rm -rf
    MKDIR = mkdir -p
    CP = cp
endif

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
ifeq ($(OS),Windows_NT)
    STATIC_TARGET = libminiscript.a
else
    STATIC_TARGET = $(TARGET)
endif

.PHONY: all clean dirs

all: dirs $(STATIC_TARGET) $(SHARED_TARGET) $(INTERPRETER)

dirs:
ifeq ($(OS),Windows_NT)
	@if not exist "$(BUILD_DIR)" $(MKDIR) $(BUILD_DIR)
	@if not exist "$(BUILD_DIR)\core" $(MKDIR) $(BUILD_DIR)\core
	@if not exist "$(BUILD_DIR)\lexer" $(MKDIR) $(BUILD_DIR)\lexer
	@if not exist "$(BUILD_DIR)\parser" $(MKDIR) $(BUILD_DIR)\parser
	@if not exist "$(BUILD_DIR)\vm" $(MKDIR) $(BUILD_DIR)\vm
	@if not exist "$(BUILD_DIR)\jit" $(MKDIR) $(BUILD_DIR)\jit
	@if not exist "$(BUILD_DIR)\api" $(MKDIR) $(BUILD_DIR)\api
	@if not exist "$(BUILD_DIR)\stdlib" $(MKDIR) $(BUILD_DIR)\stdlib
else
	@$(MKDIR) $(BUILD_DIR)/core $(BUILD_DIR)/lexer $(BUILD_DIR)/parser $(BUILD_DIR)/vm $(BUILD_DIR)/jit $(BUILD_DIR)/api $(BUILD_DIR)/stdlib
endif

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(STATIC_TARGET): $(OBJECTS)
	ar rcs $@ $^

$(SHARED_TARGET): $(OBJECTS)
ifeq ($(OS),Windows_NT)
	$(CC) -shared -o $@ $^ $(LDFLAGS) -Wl,--out-implib,libminiscript.lib
else
	$(CC) -shared -o $@ $^ $(LDFLAGS)
endif

$(INTERPRETER): $(STATIC_TARGET) src/main.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ src/main.c -L. -lminiscript $(LDFLAGS)

clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(BUILD_DIR)" rmdir /S /Q $(BUILD_DIR)
	@if exist "$(STATIC_TARGET)" del /Q $(STATIC_TARGET)
	@if exist "$(SHARED_TARGET)" del /Q $(SHARED_TARGET)
	@if exist "$(INTERPRETER)" del /Q $(INTERPRETER)
	@if exist "libminiscript.lib" del /Q libminiscript.lib
else
	$(RM) $(BUILD_DIR) $(STATIC_TARGET) $(SHARED_TARGET) $(INTERPRETER)
endif

install: all
ifeq ($(OS),Windows_NT)
	@echo "Windows installation not implemented. Copy files manually."
else
	$(CP) $(STATIC_TARGET) /usr/local/lib/
	$(CP) $(SHARED_TARGET) /usr/local/lib/
	$(CP) -r $(INCLUDE_DIR)/* /usr/local/include/
	$(CP) $(INTERPRETER) /usr/local/bin/
endif