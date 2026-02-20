# 列表推导式栈管理问题修复

## 问题描述

在实现 Python 风格的列表推导式后，发现当列表推导式后紧跟 for 循环时会出现运行时错误：
```
Runtime error: For loop index must be an integer.
```

## 问题原因

原始实现中，列表推导式使用了以下栈布局：
```
Stack: [..., iter, index, var, list]
```

其中 `iter`、`index`、`var` 是局部变量，而 `list` 只是栈上的一个值（不是局部变量）。

当调用 `end_scope_keep_top()` 清理作用域时，使用临时全局变量来保存和恢复 `list`：
1. 保存 `list` 到临时全局变量
2. 弹出 `list`
3. 弹出 3 个局部变量
4. 从临时全局变量恢复 `list`

这种方法存在问题：
- 栈管理复杂，容易出错
- 临时全局变量可能与后续代码冲突
- 在某些情况下，栈状态不一致导致后续 for 循环失败

## 解决方案

将结果列表也作为局部变量存储，简化栈管理：

### 修改前
```c
// 创建列表但不作为局部变量
emit_bytes(parser, OP_BUILD_LIST, 0);
// Stack: [..., iter, index, var, list]

// 循环体中复制列表
emit_byte(parser, OP_DUP);

// 结束时使用 end_scope_keep_top() 清理
end_scope_keep_top(parser);
```

### 修改后
```c
// 创建列表并作为局部变量
emit_bytes(parser, OP_BUILD_LIST, 0);
add_local(parser, (ms_token_t){.start = "__result__", .length = 10});
mark_initialized();
uint8_t result_slot = local_count - 1;
// Stack: [...] (所有 4 个值都是局部变量)

// 循环体中从局部变量获取列表
emit_bytes(parser, OP_GET_LOCAL, result_slot);

// 结束时先获取结果，再清理作用域
emit_bytes(parser, OP_GET_LOCAL, result_slot);
end_scope(parser);
// Stack: [..., list]
```

## 关键改进

1. **统一管理**: 所有循环相关的值（iter、index、var、result）都作为局部变量
2. **简化栈操作**: 不再需要 `end_scope_keep_top()` 和临时全局变量
3. **清晰的作用域**: 使用标准的 `end_scope()` 清理，在清理前获取结果

## 局部变量布局

```
Slot 0: __iter__   (可迭代对象)
Slot 1: __index__  (当前索引)
Slot 2: var        (循环变量)
Slot 3: __result__ (结果列表)
```

## 测试结果

所有测试通过：
- ✅ 列表推导式后跟 for 循环
- ✅ 三元表达式列表推导式后跟 for 循环
- ✅ 多个连续的列表推导式
- ✅ 列表推导式在 for 循环内
- ✅ 嵌套 for 循环

## 已知限制

- ❌ 嵌套列表推导式（如 `[[i * 3 + j for j in range(3)] for i in range(3)]`）
  - 原因：内层列表推导式的局部变量会与外层冲突
  - 解决方案：使用嵌套 for 循环代替

## 相关文件

- `src/parser/parser.c` - `list_literal()` 函数（行 ~276-470）
- `test_minimal_issue.ms` - 最小复现测试
- `test_comprehensive.ms` - 综合测试
- `LIST_COMPREHENSION_TERNARY.md` - 完整文档
