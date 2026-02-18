# MiniScript F-String 实现计划

## 功能概述

实现 Python 3 风格的 f-string 支持，允许在字符串中嵌入表达式。

## 实现方案

### 1. 词法分析 (Lexer)
- 检测 f"..." 或 f'...' 模式
- 返回 TOKEN_FSTRING 类型
- 保留原始内容（包括 f 前缀）

### 2. 语法分析 (Parser)
- 解析 f-string 内容
- 提取 {...} 中的表达式
- 转换为字符串连接操作

### 3. 代码生成
- 为每个字符串部分生成 OP_CONSTANT
- 为每个表达式生成编译代码
- 使用 OP_ADD 连接所有部分

## 示例

```miniscript
name = "World"
age = 25
msg = f"Hello {name}, you are {age} years old"
print(msg)
# 输出: Hello World, you are 25 years old
```

## 转换过程

f-string: `f"Hello {name}, you are {age} years old"`

转换为:
```miniscript
"Hello " + name + ", you are " + age + " years old"
```

## 实现步骤

1. 修改 lexer.c 的 identifier_token 函数检测 f-string
2. 修改 lexer.h 添加 TOKEN_FSTRING
3. 修改 parser.c 添加 fstring() 函数
4. 修改 parser.c 的 rules 数组添加 f-string 规则
5. 创建测试脚本验证功能
