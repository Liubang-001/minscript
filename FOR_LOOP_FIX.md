# For 循环遍历问题修复总结

## 问题描述

在实现 list, tuple, dict 的 for 循环遍历时，遇到以下问题：
1. List 元素顺序反转（5, 4, 3, 2, 1 而不是 1, 2, 3, 4, 5）
2. For 循环后的代码不执行

## 根本原因

### 问题 1：List 元素顺序反转
在 `OP_BUILD_LIST` 的实现中，使用了倒序遍历：
```c
for (int i = count - 1; i >= 0; i--) {
    ms_list_append(list, vm->stack_top[-count + i]);
}
```

### 问题 2：For 循环后代码不执行
在 `for_statement` 的实现中，局部变量的管理不正确：
- 使用了 `OP_SET_LOCAL` 来设置 iterable 和 index，这会从栈中弹出值
- 然后又添加了额外的 `OP_POP` 指令
- 导致栈不平衡，局部变量槽位与栈上的值不对应

## 解决方案

### 修复 1：List 元素顺序
将 `OP_BUILD_LIST` 改为正序遍历：
```c
for (int i = 0; i < count; i++) {
    ms_list_append(list, vm->stack_top[-count + i]);
}
```

### 修复 2：局部变量管理
正确的局部变量管理方式：
1. 将值推送到栈上
2. 调用 `add_local()` 和 `mark_initialized()`
3. 不要使用 `OP_SET_LOCAL`（它会弹出值）
4. 让值自然地留在栈上作为局部变量的存储

修改后的 `for_statement`：
```c
// 循环变量 - 初始化为 nil
emit_byte(parser, OP_NIL);
add_local(parser, var_name);
mark_initialized();
uint8_t var_slot = local_count - 1;

// Iterable - 表达式的值已经在栈上
expression(parser);
add_local(parser, (ms_token_t){.start = "__iter__", .length = 8});
mark_initialized();
uint8_t iter_slot = local_count - 1;

// Index - 推送 0 到栈上
emit_constant(parser, ms_value_int(0));
add_local(parser, (ms_token_t){.start = "__index__", .length = 9});
mark_initialized();
uint8_t index_slot = local_count - 1;
```

## 测试结果

所有集合类型的 for 循环遍历现在都正常工作：

### List 遍历
```python
for x in [1, 2, 3, 4, 5]:
    print(x)
# 输出：1 2 3 4 5
```

### Tuple 遍历
```python
for y in (10, 20, 30):
    print(y)
# 输出：10 20 30
```

### Dict 遍历（遍历键）
```python
for key in {"a": 1, "b": 2, "c": 3}:
    print(key)
# 输出：a b c（顺序可能不同）
```

### String 遍历
```python
for char in "hello":
    print(char)
# 输出：h e l l o
```

### 嵌套循环
```python
for i in [1, 2]:
    for j in [10, 20]:
        print(i + j)
# 输出：11 21 12 22
```

## 修改的文件

1. `src/vm/vm.c` - 修复 `OP_BUILD_LIST` 的元素顺序
2. `src/parser/parser.c` - 修复 `for_statement` 的局部变量管理

## 关键学习点

1. 局部变量在编译时的管理：
   - 局部变量的值存储在栈上
   - `add_local()` 只是记录变量名和槽位
   - `mark_initialized()` 标记变量已初始化
   - 不需要使用 `OP_SET_LOCAL`（除非是赋值操作）

2. 栈管理：
   - 每个局部变量在栈上占一个槽位
   - `end_scope()` 会为每个局部变量发出 `OP_POP` 来清理栈
   - 必须确保栈上的值与局部变量槽位一一对应

3. 字节码生成：
   - 循环体内使用 `declaration()` 而不是 `statement()`
   - 这样可以支持在循环体内声明变量
