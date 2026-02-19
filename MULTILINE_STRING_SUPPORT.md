# 多行字符串和空行支持

## 概述

本次更新添加了两个重要功能：
1. 多行字符串支持（三引号 `"""`）
2. 修复了代码块开始处的空行问题

## 1. 多行字符串支持

### 功能描述

MiniScript 现在支持 Python 3 风格的多行字符串，使用三个双引号 `"""` 来定义。

### 语法

```python
# 基本多行字符串
text = """第一行
第二行
第三行"""

# 函数文档字符串
def my_function():
    """这是函数的文档字符串
    可以用来描述函数的功能
    支持多行"""
    pass

# 多行注释（实际上是字符串字面量）
"""
这是一个多行注释
在 Python 中常用于注释
"""
```

### 特性

- ✅ 支持跨越多行
- ✅ 保留换行符和缩进
- ✅ 可用作文档字符串
- ✅ 可用作多行注释
- ✅ 支持空的多行字符串 `""""""`

### 实现细节

#### 词法分析器 (lexer.c)

修改了 `string_token()` 函数来检测和处理三引号：

```c
static ms_token_t string_token(ms_lexer_t* lexer) {
    // 检查是否是三引号字符串
    bool is_triple = false;
    if (peek(lexer) == '"' && peek_next(lexer) == '"') {
        is_triple = true;
        advance(lexer);  // 第二个引号
        advance(lexer);  // 第三个引号
    }
    
    if (is_triple) {
        // 三引号字符串 - 可以跨多行
        while (!is_at_end(lexer)) {
            // 检查是否遇到结束的三引号
            if (peek(lexer) == '"' && peek_next(lexer) == '"' && ...) {
                // 找到结束的三引号
                advance(lexer);
                advance(lexer);
                advance(lexer);
                return make_token(lexer, TOKEN_STRING);
            }
            
            if (peek(lexer) == '\n') {
                lexer->line++;
                lexer->column = 0;
            }
            advance(lexer);
        }
        
        return error_token(lexer, "Unterminated triple-quoted string.");
    }
    // ... 普通字符串处理
}
```

#### 解析器 (parser.c)

修改了 `string()` 函数来正确处理三引号：

```c
static void string(ms_parser_t* parser) {
    // 检查是否是三引号字符串
    int quote_len = 1;  // 默认单引号
    if (parser->previous.length >= 6 &&
        parser->previous.start[0] == '"' &&
        parser->previous.start[1] == '"' &&
        parser->previous.start[2] == '"') {
        quote_len = 3;  // 三引号
    }
    
    // 去掉引号
    int str_len = parser->previous.length - 2 * quote_len;
    char* str = malloc(str_len + 1);
    memcpy(str, parser->previous.start + quote_len, str_len);
    str[str_len] = '\0';
    emit_constant(parser, ms_value_string(str));
    free(str);
}
```

### 使用示例

```python
# 示例 1: 多行文本
message = """欢迎使用 MiniScript！
这是一个支持 Python 3 语法的脚本语言。
您可以使用多行字符串来编写长文本。"""
print(message)

# 示例 2: 代码模板
template = """def hello():
    print("Hello, World!")
    return True"""
print(template)

# 示例 3: 函数文档
def calculate(a, b):
    """计算两个数的和
    
    参数:
        a: 第一个数
        b: 第二个数
    
    返回:
        两数之和
    """
    return a + b
```

## 2. 空行问题修复

### 问题描述

之前的实现中，如果在代码块（函数、if、while、for、with）的开始处有空行，会导致编译错误：

```
[line X] Error at '': Expect indentation after ':'.
```

### 问题原因

解析器在 `consume(TOKEN_NEWLINE)` 之后立即 `consume(TOKEN_INDENT)`，但如果有空行，词法分析器会跳过空行后才生成 INDENT token，导致解析器在错误的位置期望 INDENT。

### 解决方案

在所有代码块的解析中，在 `consume(TOKEN_INDENT)` 之前添加 `skip_newlines()` 调用：

```c
// 函数定义
consume(parser, TOKEN_COLON, "Expect ':' after function signature.");
consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
skip_newlines(parser);  // 跳过空行
consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");

// if 语句
consume(parser, TOKEN_COLON, "Expect ':' after if condition.");
consume(parser, TOKEN_NEWLINE, "Expect newline after ':'.");
skip_newlines(parser);  // 跳过空行
consume(parser, TOKEN_INDENT, "Expect indentation after ':'.");

// 类似地修复了 elif, else, while, for, with
```

### 修复的代码块

- ✅ 函数定义 (`def`)
- ✅ if 语句
- ✅ elif 语句
- ✅ else 语句
- ✅ while 循环
- ✅ for 循环
- ✅ with 语句

### 测试示例

```python
# 现在这些都可以正常工作

def test1():

    a = 1
    print(a)

def test2():
    
    # 注释
    b = 2
    print(b)

if True:

    print("if 块中的空行")

while False:

    pass

for i in range(5):

    print(i)
```

## 测试文件

### 多行字符串测试
- `test_multiline_string.ms` - 基本多行字符串测试
- `test_multiline_complete.ms` - 完整的多行字符串测试

### 空行测试
- `test_empty_simple.ms` - 简单的空行测试
- `test_empty_func.ms` - 函数中的空行测试
- `test_empty_lines_comprehensive.ms` - 完整的空行测试

## 运行测试

```bash
# 测试多行字符串
.\miniscript.exe test_multiline_complete.ms

# 测试空行
.\miniscript.exe test_empty_simple.ms
.\miniscript.exe test_empty_func.ms
```

## 与 Python 3 的兼容性

### 已实现
- ✅ 三引号多行字符串 `"""`
- ✅ 多行字符串中的换行符保留
- ✅ 函数文档字符串
- ✅ 多行注释（字符串字面量）
- ✅ 代码块开始处的空行

### 未实现
- ❌ 单引号三引号 `'''`
- ❌ 原始字符串 `r"""`
- ❌ 字节字符串 `b"""`
- ❌ Unicode 字符串 `u"""`

## 总结

本次更新显著提升了 MiniScript 的 Python 3 兼容性：

1. **多行字符串支持**让代码更易读，特别是对于长文本和文档字符串
2. **空行问题修复**让代码格式更灵活，符合 Python 的编码风格

这两个功能使得 MiniScript 更接近真正的 Python 3 体验。
