# 列表推导式 + 三元表达式实现

## 概述

MiniScript 现在完全支持 Python 风格的列表推导式和三元表达式，包括它们的组合使用。

## 语法

### 三元表达式
```python
result = value_if_true if condition else value_if_false
```

### 列表推导式
```python
# 简单映射
result = [expr for var in iterable]

# 带三元表达式
result = [value_if_true if condition else value_if_false for var in iterable]
```

## 示例

### 1. 基本三元表达式
```python
x = 10
result = "大于5" if x > 5 else "小于等于5"
print(result)  # 输出: 大于5
```

### 2. 简单列表推导式
```python
numbers = [1, 2, 3, 4, 5]
squared = [n * n for n in numbers]
print(squared)  # 输出: [1, 4, 9, 16, 25]
```

### 3. 列表推导式 + 三元表达式
```python
numbers = [1, 2, 3, 4, 5]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
print(result_list)  # 输出: ["奇数", "偶数", "奇数", "偶数", "奇数"]
```

### 4. 复杂条件
```python
scores = [85, 92, 58, 73, 45, 96]
grades = ["及格" if s >= 60 else "不及格" for s in scores]
print(grades)  # 输出: ["及格", "及格", "不及格", "及格", "不及格", "及格"]
```

### 5. 嵌套三元表达式
```python
ages = [5, 15, 25, 65, 75]
categories = [
    "儿童" if age < 12 else (
        "青少年" if age < 18 else (
            "成人" if age < 60 else "老年"
        )
    ) for age in ages
]
print(categories)  # 输出: ["儿童", "青少年", "成人", "老年", "老年"]
```

### 6. 数值计算
```python
values = [1, 2, 3, 4, 5]
doubled_or_tripled = [n * 2 if n % 2 == 0 else n * 3 for n in values]
print(doubled_or_tripled)  # 输出: [3, 4, 9, 8, 15]
```

### 7. 使用 range
```python
evens = ["偶" if i % 2 == 0 else "奇" for i in range(10)]
print(evens)  # 输出: ["偶", "奇", "偶", "奇", "偶", "奇", "偶", "奇", "偶", "奇"]
```

## 实现细节

### 三元表达式
- 优先级: `PREC_TERNARY`
- 操作码: `OP_TERNARY`
- 语法: `value_if_true if condition else value_if_false`
- 栈操作: 弹出三个值（false值、条件、true值），根据条件推送相应的值

### 列表推导式
- 在解析时检测 `[expr for var in iterable]` 模式
- 创建新的作用域用于循环变量
- 使用局部变量存储:
  - `__iter__`: 可迭代对象
  - `__index__`: 当前索引
  - `var`: 循环变量
  - `__result__`: 结果列表
- 使用 `OP_FOR_ITER_LOCAL` 操作码进行迭代
- 在循环体内重新解析表达式（支持复杂表达式，包括三元表达式）
- 在作用域结束前获取结果列表，然后清理所有局部变量

### 关键技术
1. **表达式重新解析**: 保存表达式的源代码位置，在循环体内（循环变量已定义）重新解析
2. **字节码回滚**: 如果检测到 `for` 关键字，回滚之前生成的字节码
3. **临时词法分析器**: 为表达式创建临时 lexer 进行重新解析
4. **作用域管理**: 将结果列表也作为局部变量，在作用域结束前获取并返回

## 支持的可迭代类型
- 列表 (list)
- 字典 (dict) - 迭代键
- 元组 (tuple)
- 字符串 (string) - 迭代字符
- range 对象

## 测试文件
- `test_ternary.ms` - 三元表达式测试
- `test_listcomp_simple.ms` - 简单列表推导式测试
- `test_listcomp_ternary.ms` - 列表推导式 + 三元表达式综合测试
- `test_len.ms` - 列表长度和元素访问测试

## 与 Python 的兼容性
此实现完全兼容 Python 的语法：
```python
# Python 代码
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]

# MiniScript 代码（完全相同）
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
```

## 性能考虑
- 列表推导式使用编译时生成的循环代码，性能接近手写循环
- 三元表达式在运行时只评估一个分支，避免不必要的计算
- 表达式重新解析只在编译时发生一次

## 已知限制
- 当前不支持列表推导式中的 `if` 过滤器（如 `[x for x in list if condition]`）
- 不支持多重 `for` 循环（如 `[x+y for x in list1 for y in list2]`）
- 不支持嵌套列表推导式（如 `[[i * 3 + j for j in range(3)] for i in range(3)]`）
  - 可以使用嵌套 for 循环代替

## 未来改进
- 添加列表推导式中的 `if` 过滤器支持
- 添加多重 `for` 循环支持
- 添加嵌套列表推导式支持
- 添加字典推导式和集合推导式支持
