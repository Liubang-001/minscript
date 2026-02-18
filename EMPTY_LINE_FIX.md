# 空行处理问题修复

## 问题描述

在函数体、if/elif/else、while、for 等代码块中，如果存在空行，会导致编译错误：

```
[line X] Error at '': Expect expression.
Compile error
```

### 问题示例

```python
def basic_syntax():
    c = "Hello"
    
    print(c)  # 空行后的代码会报错

basic_syntax()
```

## 根本原因

问题出在两个地方：

### 1. 词法分析器（lexer.c）

在处理行首缩进时，遇到空行会递归调用 `ms_lexer_scan_token(lexer)`，这会导致：
- 递归深度增加
- 某些情况下跳过了必要的缩进检查
- 空行后的代码缩进处理不正确

### 2. 解析器（parser.c）

在解析代码块（函数体、if/while/for 等）时，没有跳过多余的换行符，导致：
- 遇到空行后的换行符时，`statement()` 被调用
- `statement()` 无法匹配任何语句类型，调用 `expression_statement()`
- `expression_statement()` 立即调用 `expression()`
- `expression()` 遇到 NEWLINE token，报错 "Expect expression"

## 解决方案

### 1. 修复词法分析器

将递归调用改为循环处理，避免递归深度问题：

```c
// 处理行首的缩进
if (lexer->at_line_start) {
    lexer->at_line_start = false;
    
    // 跳过所有空行和注释行
    while (true) {
        // 计算当前行的缩进
        int indent = 0;
        while (peek(lexer) == ' ' || peek(lexer) == '\t') {
            if (peek(lexer) == ' ') indent++;
            else indent += 4;
            advance(lexer);
        }
        
        lexer->start = lexer->current;
        
        // 检查是否是空行或注释行
        if (peek(lexer) == '\n') {
            // 空行，跳过
            advance(lexer);
            lexer->line++;
            lexer->column = 1;
            continue;  // 继续处理下一行
        } else if (peek(lexer) == '#') {
            // 注释行，跳过到行尾
            while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                advance(lexer);
            }
            if (peek(lexer) == '\n') {
                advance(lexer);
                lexer->line++;
                lexer->column = 1;
                continue;
            }
        } else if (is_at_end(lexer)) {
            return make_token(lexer, TOKEN_EOF);
        }
        
        // 不是空行或注释行，处理缩进
        // ... 缩进处理逻辑 ...
        break;
    }
}
```

### 2. 修复解析器

添加辅助函数跳过换行符，并在所有代码块解析循环中使用：

```c
// 添加辅助函数
static void skip_newlines(ms_parser_t* parser) {
    while (match(parser, TOKEN_NEWLINE)) {
        // Skip empty lines
    }
}

// 在所有代码块中使用
while (!check(parser, TOKEN_DEDENT) && !check(parser, TOKEN_EOF)) {
    skip_newlines(parser);
    if (check(parser, TOKEN_DEDENT) || check(parser, TOKEN_EOF)) break;
    declaration(parser);
}
```

## 修改的文件

1. **src/lexer/lexer.c**
   - 修改行首缩进处理逻辑
   - 将递归改为循环
   - 正确处理连续的空行和注释行

2. **src/parser/parser.c**
   - 添加 `skip_newlines()` 辅助函数
   - 修改所有代码块解析循环：
     - `if_statement()` - if 块
     - `if_statement()` - elif 块
     - `if_statement()` - else 块
     - `while_statement()` - while 块
     - `for_statement()` - for 块
     - `with_statement()` - with 块
     - `function_declaration()` - 函数体

## 测试结果

所有测试用例都通过：

### 基本测试
```python
def basic_syntax():
    c = "Hello"
    
    print(c)

basic_syntax()
# 输出: Hello
```

### 多个空行
```python
def test():
    a = 1
    
    b = 2
    
    c = 3
    print(a, b, c)

test()
# 输出: 1 2 3
```

### if 语句中的空行
```python
def test_if():
    x = 5
    
    if x > 0:
        print("positive")
        
        print("still positive")
    
    print("done")

test_if()
# 输出:
# positive
# still positive
# done
```

### while 循环中的空行
```python
def test_while():
    i = 0
    
    while i < 3:
        print(i)
        
        i = i + 1
    
    print("while done")

test_while()
# 输出:
# 0
# 1
# 2
# while done
```

### for 循环中的空行
```python
def test_for():
    nums = [1, 2, 3]
    
    for n in nums:
        print(n)
        
        print(n * 2)
    
    print("for done")

test_for()
# 输出:
# 1
# 2
# 2
# 4
# 3
# 6
# for done
```

## 影响范围

修复后，以下所有场景都能正确处理空行：

✓ 函数体内的空行
✓ if/elif/else 块内的空行
✓ while 循环内的空行
✓ for 循环内的空行
✓ with 语句内的空行
✓ 连续多个空行
✓ 空行与注释行混合

## 兼容性

- 修复不影响现有代码
- 完全向后兼容
- 符合 Python 3 的空行处理行为

## 总结

通过修改词法分析器和解析器，成功解决了代码块中空行导致的编译错误问题。现在可以像 Python 一样自由地在代码中使用空行来提高可读性。
