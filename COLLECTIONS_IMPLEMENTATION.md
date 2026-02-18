# MiniScript Collections 实现计划

## 功能概述

实现 Python 风格的集合类型支持：
- List（列表）
- Dict（字典）
- Tuple（元组）
- For 循环遍历

## 实现步骤

### 1. 数据结构定义
- List: 动态数组
- Dict: 哈希表
- Tuple: 不可变列表

### 2. 词法分析
- `[` `]` - 列表
- `{` `}` - 字典
- `(` `)` - 元组
- `for` `in` - for 循环

### 3. 语法分析
- 列表字面量: `[1, 2, 3]`
- 字典字面量: `{"key": value}`
- 元组字面量: `(1, 2, 3)`
- For 循环: `for x in list:`

### 4. 虚拟机
- OP_BUILD_LIST - 构建列表
- OP_BUILD_DICT - 构建字典
- OP_INDEX_GET - 获取元素
- OP_INDEX_SET - 设置元素
- For 循环执行

### 5. 内置函数
- len() - 获取长度
- append() - 添加元素
- pop() - 移除元素
- keys() - 获取字典键
- values() - 获取字典值
- items() - 获取字典项

## 文件修改

- `include/miniscript.h` - 添加集合类型定义
- `src/vm/vm.h` - 添加集合结构
- `src/vm/vm.c` - 实现集合操作
- `src/parser/parser.c` - 解析集合和 for 循环
- `src/lexer/lexer.c` - 词法分析
