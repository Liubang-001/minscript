# break 和 continue 支持

## 概述

MiniScript 现在完全支持 Python 3 风格的 `break` 和 `continue` 语句，用于控制循环流程。

## 功能描述

### break 语句

`break` 语句用于立即退出当前循环（while 或 for）。

```python
# while 循环中的 break
i = 0
while i < 10:
    if i == 5:
        break
    print(i)
    i = i + 1
# 输出: 0 1 2 3 4

# for 循环中的 break
for i in range(10):
    if i == 5:
        break
    print(i)
# 输出: 0 1 2 3 4
```

### continue 语句

`continue` 语句用于跳过当前迭代的剩余部分，直接进入下一次迭代。

```python
# while 循环中的 continue
i = 0
while i < 5:
    i = i + 1
    if i == 3:
        continue
    print(i)
# 输出: 1 2 4 5

# for 循环中的 continue
for i in range(5):
    if i == 2:
        continue
    print(i)
# 输出: 0 1 3 4
```

## 语法规则

### 1. 只能在循环中使用

`break` 和 `continue` 只能在 `while` 或 `for` 循环内部使用。在循环外使用会导致编译错误：

```python
# 错误示例
break  # Error: 'break' outside loop.
continue  # Error: 'continue' outside loop.
```

### 2. 嵌套循环

在嵌套循环中，`break` 和 `continue` 只影响最内层的循环：

```python
for i in range(3):
    print(f"外层 i = {i}")
    for j in range(5):
        if j == 3:
            break  # 只退出内层循环
        print(f"  内层 j = {j}")
```

### 3. 混合使用

可以在同一个循环中混合使用 `break` 和 `continue`：

```python
for i in range(10):
    if i % 2 == 0:
        continue  # 跳过偶数
    if i > 7:
        break  # 大于 7 时退出
    print(i)
# 输出: 1 3 5 7
```

## 实现细节

### 编译时处理

1. **循环深度跟踪**
   - 使用 `loop_depth` 变量跟踪当前循环嵌套深度
   - 进入循环时 `loop_depth++`，退出时 `loop_depth--`

2. **跳转位置记录**
   - `break_jumps[]` 数组记录所有 break 语句的跳转位置
   - `continue_jumps[]` 数组记录所有 continue 语句的跳转位置

3. **跳转修补**
   - 在循环结束时，将所有 break 跳转修补到循环结束位置
   - 在循环回跳前，将所有 continue 跳转修补到循环开始位置

### 代码结构

```c
// 解析器中的全局变量
static int loop_depth = 0;
static int break_jumps[256];
static int break_count = 0;
static int continue_jumps[256];
static int continue_count = 0;

// while 循环处理
static void while_statement(ms_parser_t* parser) {
    loop_depth++;
    int saved_break_count = break_count;
    int saved_continue_count = continue_count;
    break_count = 0;
    continue_count = 0;
    
    // ... 循环体解析 ...
    
    // patch continue 跳转到循环开始
    for (int i = 0; i < continue_count; i++) {
        patch_jump(parser, continue_jumps[i]);
    }
    
    emit_loop(parser, loop_start);
    
    // patch break 跳转到循环结束
    for (int i = 0; i < break_count; i++) {
        patch_jump(parser, break_jumps[i]);
    }
    
    loop_depth--;
    break_count = saved_break_count;
    continue_count = saved_continue_count;
}
```

### 字节码生成

- `break` 生成 `OP_JUMP` 指令，跳转到循环结束
- `continue` 生成 `OP_JUMP` 指令，跳转到循环开始
- 使用现有的 `OP_JUMP` 操作码，无需新增 VM 指令

## 测试用例

### 基本测试

```python
# test_break_simple.ms

# while break
i = 0
while i < 10:
    if i == 3:
        break
    print(i)
    i = i + 1

# for break
for i in range(10):
    if i == 3:
        break
    print(i)

# while continue
i = 0
while i < 5:
    i = i + 1
    if i == 3:
        continue
    print(i)

# for continue
for i in range(5):
    if i == 2:
        continue
    print(i)
```

### 嵌套循环测试

```python
# 嵌套循环中的 break
for i in range(3):
    print(f"外层 i = {i}")
    for j in range(5):
        if j == 3:
            break
        print(f"  内层 j = {j}")

# 嵌套循环中的 continue
for i in range(3):
    if i == 1:
        continue
    print(f"外层 i = {i}")
    for j in range(3):
        if j == 1:
            continue
        print(f"  内层 j = {j}")
```

### 错误检测测试

```python
# 应该报错：在循环外使用 break
break  # Error: 'break' outside loop.

# 应该报错：在循环外使用 continue
continue  # Error: 'continue' outside loop.
```

## 与 Python 3 的兼容性

### 已实现 ✅
- ✅ while 循环中的 break
- ✅ while 循环中的 continue
- ✅ for 循环中的 break
- ✅ for 循环中的 continue
- ✅ 嵌套循环支持
- ✅ 编译时错误检测（循环外使用）
- ✅ 混合使用 break 和 continue

### 未实现 ❌
- ❌ for/while 的 else 子句（Python 特性：循环正常结束时执行）

## 性能

- **编译时开销**: 最小（仅增加跳转位置记录）
- **运行时开销**: 无（使用标准的跳转指令）
- **内存开销**: 每个循环最多 256 个 break/continue 语句

## 使用示例

### 示例 1: 查找元素

```python
# 在列表中查找元素
numbers = [1, 3, 5, 7, 9, 11, 13]
target = 7
found = False

for num in numbers:
    if num == target:
        found = True
        break
    print(f"检查 {num}")

if found:
    print(f"找到 {target}")
else:
    print(f"未找到 {target}")
```

### 示例 2: 跳过特定值

```python
# 打印 1-10 中的奇数
for i in range(1, 11):
    if i % 2 == 0:
        continue
    print(i)
```

### 示例 3: 输入验证

```python
# 输入验证循环
while True:
    value = input("请输入一个正数（输入 0 退出）: ")
    num = int(value)
    
    if num == 0:
        break
    
    if num < 0:
        print("错误：必须是正数")
        continue
    
    print(f"你输入了: {num}")
```

## 总结

MiniScript 现在完全支持 Python 3 的 break 和 continue 语句，包括：

1. ✅ 在 while 和 for 循环中使用
2. ✅ 嵌套循环支持
3. ✅ 编译时错误检测
4. ✅ 与 Python 3 完全兼容的行为

这使得 MiniScript 的循环控制功能更加完善，可以编写更复杂的循环逻辑。
