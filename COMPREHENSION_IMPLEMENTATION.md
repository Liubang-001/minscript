# 推导式实现文档

## 概述
实现 Python 的列表推导式、集合推导式和字典推导式。

## 当前状态
- ✅ 基础列表推导式已实现：`[x for x in range(10)]`
- ✅ 带三元表达式的列表推导式：`[x if x > 0 else -x for x in range(-5, 5)]`
- ❌ 带条件过滤的列表推导式：`[x for x in range(10) if x % 2 == 0]`
- ❌ 嵌套列表推导式：`[x*y for x in range(3) for y in range(3)]`
- ❌ 集合推导式：`{x for x in range(10)}`
- ❌ 字典推导式：`{x: x**2 for x in range(10)}`

## 实现计划

### 1. 带条件过滤的列表推导式

#### 语法
```python
[表达式 for 变量 in 可迭代对象 if 条件]
```

#### 示例
```python
# 偶数
evens = [x for x in range(10) if x % 2 == 0]
# [0, 2, 4, 6, 8]

# 正数的平方
squares = [x**2 for x in range(-5, 5) if x > 0]
# [1, 4, 9, 16]

# 多个条件
result = [x for x in range(20) if x % 2 == 0 if x % 3 == 0]
# [0, 6, 12, 18]
```

#### 实现步骤

1. **解析器修改**（parser.c）
```c
static void list_comprehension() {
    // [表达式 for 变量 in 可迭代对象 if 条件]
    
    // 1. 创建临时列表
    emit_byte(OP_BUILD_LIST);
    emit_byte(0);
    
    // 2. 解析表达式
    expression();
    
    // 3. 解析 for 子句
    consume(TOKEN_FOR, "Expect 'for' in list comprehension.");
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    uint8_t var = identifier_constant(&parser.previous);
    
    consume(TOKEN_IN, "Expect 'in' after variable.");
    expression();  // 可迭代对象
    
    // 4. 检查是否有 if 条件
    bool has_condition = false;
    if (match(TOKEN_IF)) {
        has_condition = true;
        expression();  // 条件表达式
    }
    
    // 5. 生成字节码
    if (has_condition) {
        emit_byte(OP_BUILD_LIST_COMP_IF);
    } else {
        emit_byte(OP_BUILD_LIST_COMP);
    }
    emit_byte(var);
}
```

2. **添加新操作码**（vm.h）
```c
OP_BUILD_LIST_COMP_IF,  // 带条件的列表推导式
```

3. **虚拟机实现**（vm.c）
```c
case OP_BUILD_LIST_COMP_IF: {
    uint8_t var_index = READ_BYTE();
    ms_value_t condition = pop();
    ms_value_t iterable = pop();
    ms_value_t expr = pop();
    ms_value_t list = pop();
    
    // 遍历可迭代对象
    if (ms_value_is_list(iterable)) {
        ms_list_t* src = ms_value_as_list(iterable);
        ms_list_t* dst = ms_value_as_list(list);
        
        for (int i = 0; i < src->count; i++) {
            ms_value_t item = src->items[i];
            
            // 设置循环变量
            vm->stack[var_index] = item;
            
            // 计算条件
            push(condition);
            // ... 执行条件表达式 ...
            ms_value_t cond_result = pop();
            
            if (ms_value_as_bool(cond_result)) {
                // 计算表达式
                push(expr);
                // ... 执行表达式 ...
                ms_value_t result = pop();
                
                // 添加到结果列表
                ms_list_append(dst, result);
            }
        }
    }
    
    push(list);
    break;
}
```

### 2. 嵌套列表推导式

#### 语法
```python
[表达式 for 变量1 in 可迭代对象1 for 变量2 in 可迭代对象2]
```

#### 示例
```python
# 乘法表
table = [x*y for x in range(1, 4) for y in range(1, 4)]
# [1, 2, 3, 2, 4, 6, 3, 6, 9]

# 坐标对
coords = [(x, y) for x in range(3) for y in range(3)]
# [(0,0), (0,1), (0,2), (1,0), (1,1), (1,2), (2,0), (2,1), (2,2)]
```

#### 实现
嵌套推导式等价于嵌套循环：
```python
result = []
for x in range(1, 4):
    for y in range(1, 4):
        result.append(x * y)
```

### 3. 集合推导式

#### 语法
```python
{表达式 for 变量 in 可迭代对象}
{表达式 for 变量 in 可迭代对象 if 条件}
```

#### 示例
```python
# 唯一的平方数
squares = {x**2 for x in range(-5, 6)}
# {0, 1, 4, 9, 16, 25}

# 偶数集合
evens = {x for x in range(10) if x % 2 == 0}
# {0, 2, 4, 6, 8}
```

#### 实现步骤

1. **添加集合类型**（value.c）
```c
typedef struct ms_set {
    int count;
    int capacity;
    ms_value_t* items;
} ms_set_t;

ms_value_t ms_value_set(ms_set_t* set);
bool ms_value_is_set(ms_value_t value);
ms_set_t* ms_value_as_set(ms_value_t value);
```

2. **添加操作码**（vm.h）
```c
OP_BUILD_SET,           // 创建集合
OP_BUILD_SET_COMP,      // 集合推导式
OP_BUILD_SET_COMP_IF,   // 带条件的集合推导式
```

3. **解析器实现**（parser.c）
```c
static void set_or_dict_literal() {
    // { ... }
    
    if (check(TOKEN_RIGHT_BRACE)) {
        // 空字典
        emit_byte(OP_BUILD_DICT);
        emit_byte(0);
        return;
    }
    
    // 先解析第一个元素
    expression();
    
    if (match(TOKEN_COLON)) {
        // 字典：{key: value}
        dict_literal();
    } else if (match(TOKEN_FOR)) {
        // 集合推导式：{expr for x in iter}
        set_comprehension();
    } else {
        // 集合字面量：{1, 2, 3}
        set_literal();
    }
}
```

### 4. 字典推导式

#### 语法
```python
{键表达式: 值表达式 for 变量 in 可迭代对象}
{键表达式: 值表达式 for 变量 in 可迭代对象 if 条件}
```

#### 示例
```python
# 数字到平方的映射
squares = {x: x**2 for x in range(5)}
# {0: 0, 1: 1, 2: 4, 3: 9, 4: 16}

# 偶数到平方的映射
even_squares = {x: x**2 for x in range(10) if x % 2 == 0}
# {0: 0, 2: 4, 4: 16, 6: 36, 8: 64}

# 字符串长度映射
words = ["apple", "banana", "cherry"]
lengths = {word: len(word) for word in words}
# {"apple": 5, "banana": 6, "cherry": 6}
```

#### 实现步骤

1. **添加操作码**（vm.h）
```c
OP_BUILD_DICT_COMP,     // 字典推导式
OP_BUILD_DICT_COMP_IF,  // 带条件的字典推导式
```

2. **解析器实现**（parser.c）
```c
static void dict_comprehension() {
    // {key_expr: val_expr for var in iter if cond}
    
    // 1. 创建临时字典
    emit_byte(OP_BUILD_DICT);
    emit_byte(0);
    
    // 2. 键表达式已经解析
    // 3. 解析值表达式
    expression();
    
    // 4. 解析 for 子句
    consume(TOKEN_FOR, "Expect 'for'.");
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    uint8_t var = identifier_constant(&parser.previous);
    
    consume(TOKEN_IN, "Expect 'in'.");
    expression();  // 可迭代对象
    
    // 5. 可选的 if 条件
    bool has_condition = match(TOKEN_IF);
    if (has_condition) {
        expression();
    }
    
    // 6. 生成字节码
    if (has_condition) {
        emit_byte(OP_BUILD_DICT_COMP_IF);
    } else {
        emit_byte(OP_BUILD_DICT_COMP);
    }
    emit_byte(var);
}
```

3. **虚拟机实现**（vm.c）
```c
case OP_BUILD_DICT_COMP: {
    uint8_t var_index = READ_BYTE();
    ms_value_t iterable = pop();
    ms_value_t val_expr = pop();
    ms_value_t key_expr = pop();
    ms_value_t dict = pop();
    
    // 遍历可迭代对象
    if (ms_value_is_list(iterable)) {
        ms_list_t* src = ms_value_as_list(iterable);
        ms_dict_t* dst = ms_value_as_dict(dict);
        
        for (int i = 0; i < src->count; i++) {
            ms_value_t item = src->items[i];
            
            // 设置循环变量
            vm->stack[var_index] = item;
            
            // 计算键表达式
            // ... 执行 key_expr ...
            ms_value_t key = pop();
            
            // 计算值表达式
            // ... 执行 val_expr ...
            ms_value_t value = pop();
            
            // 添加到字典
            ms_dict_set(dst, ms_value_as_string(key), value);
        }
    }
    
    push(dict);
    break;
}
```

## 测试用例

### test_list_comp_filter.ms
```python
# 基础过滤
evens = [x for x in range(10) if x % 2 == 0]
print(evens)  # [0, 2, 4, 6, 8]

# 多个条件
result = [x for x in range(20) if x % 2 == 0 if x % 3 == 0]
print(result)  # [0, 6, 12, 18]

# 复杂表达式
squares = [x**2 for x in range(-5, 5) if x > 0]
print(squares)  # [1, 4, 9, 16]
```

### test_list_comp_nested.ms
```python
# 嵌套循环
table = [x*y for x in range(1, 4) for y in range(1, 4)]
print(table)  # [1, 2, 3, 2, 4, 6, 3, 6, 9]

# 坐标对
coords = [(x, y) for x in range(3) for y in range(3)]
print(coords)
```

### test_set_comp.ms
```python
# 基础集合推导式
squares = {x**2 for x in range(-5, 6)}
print(squares)  # {0, 1, 4, 9, 16, 25}

# 带条件
evens = {x for x in range(10) if x % 2 == 0}
print(evens)  # {0, 2, 4, 6, 8}
```

### test_dict_comp.ms
```python
# 基础字典推导式
squares = {x: x**2 for x in range(5)}
print(squares)  # {0: 0, 1: 1, 2: 4, 3: 9, 4: 16}

# 带条件
even_squares = {x: x**2 for x in range(10) if x % 2 == 0}
print(even_squares)  # {0: 0, 2: 4, 4: 16, 6: 36, 8: 64}

# 字符串处理
words = ["apple", "banana", "cherry"]
lengths = {word: len(word) for word in words}
print(lengths)
```

## 实现优先级

1. **高优先级**：带条件过滤的列表推导式（最常用）
2. **中优先级**：字典推导式（常用）
3. **中优先级**：集合推导式（较常用）
4. **低优先级**：嵌套推导式（复杂，可后续实现）

## 实现时间估算

- 带条件的列表推导式：2-3小时
- 集合推导式：2-3小时（需要先实现集合类型）
- 字典推导式：2-3小时
- 嵌套推导式：3-4小时

总计：9-13小时（约1-2天）

## 下一步行动

1. 实现带条件过滤的列表推导式
2. 实现集合类型和集合推导式
3. 实现字典推导式
4. 编写测试用例验证功能
