# Assert 和 Del 语句实现完成

## 实现日期
2024年2月20日

## 实现内容

成功实现了 Python 3 的 assert 和 del 语句！

### 1. Assert 语句

#### 语法
```python
assert condition
assert condition, "error message"
```

#### 示例
```python
x = 10
assert x == 10
assert x > 5
assert x < 100, "x should be less than 100"
```

#### 测试结果
所有测试通过 ✅

### 2. Del 语句

#### 语法
```python
del variable
```

#### 示例
```python
temp = 42
del temp

a = 1
b = 2
c = 3
del b  # 删除 b
```

#### 测试结果
所有测试通过 ✅

## 实现步骤

### Assert 语句

#### 1. 词法分析器
- ✅ TOKEN_ASSERT 已添加

#### 2. 解析器 (src/parser/parser.c)
在 `statement()` 函数中添加：
```c
} else if (match(parser, TOKEN_ASSERT)) {
    // assert 语句: assert condition [, message]
    expression(parser);  // 解析条件表达式
    
    // 检查是否有可选的错误消息
    if (match(parser, TOKEN_COMMA)) {
        expression(parser);  // 解析错误消息
        emit_byte(parser, OP_ASSERT);
    } else {
        emit_byte(parser, OP_NIL);  // 没有消息，推送 nil
        emit_byte(parser, OP_ASSERT);
    }
    
    // 消费语句后的换行符
    if (match(parser, TOKEN_NEWLINE)) {
        // 换行符已消费
    } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
        error(parser, "Expect newline after 'assert'.");
    }
}
```

#### 3. 虚拟机 (src/vm/vm.c)
添加 OP_ASSERT 处理：
```c
case OP_ASSERT: {
    // assert 语句: 栈上有 [condition, message]
    ms_value_t message = ms_vm_pop(vm);
    ms_value_t condition = ms_vm_pop(vm);
    
    // 检查条件是否为真
    if (!ms_value_as_bool(condition)) {
        // 断言失败
        if (ms_value_is_string(message)) {
            runtime_error(vm, "AssertionError: %s", ms_value_as_string(message));
        } else {
            runtime_error(vm, "AssertionError");
        }
        return MS_RESULT_RUNTIME_ERROR;
    }
    break;
}
```

### Del 语句

#### 1. 词法分析器 (src/lexer/lexer.c)
修复关键字识别：
```c
case 'd': 
    if (lexer->current - lexer->start > 1) {
        switch (lexer->start[1]) {
            case 'e': 
                if (lexer->current - lexer->start == 3) {
                    if (lexer->start[2] == 'f') return TOKEN_DEF;
                    if (lexer->start[2] == 'l') return TOKEN_DEL;
                }
                break;
        }
    }
    return TOKEN_IDENTIFIER;
```

#### 2. 解析器 (src/parser/parser.c)
在 `declaration()` 函数中添加：
```c
} else if (match(parser, TOKEN_DEL)) {
    // del 语句: del variable
    if (!match(parser, TOKEN_IDENTIFIER)) {
        error(parser, "Expect variable name after 'del'.");
        return;
    }
    
    ms_token_t name = parser->previous;
    uint8_t name_index = add_name(name.start, name.length);
    
    int arg = resolve_local(parser, &name);
    if (arg != -1) {
        error(parser, "Cannot delete local variable.");
        return;
    }
    
    emit_bytes(parser, OP_DELETE, name_index);
    
    // 消费语句后的换行符
    if (match(parser, TOKEN_NEWLINE)) {
        // 换行符已消费
    } else if (!check(parser, TOKEN_EOF) && !check(parser, TOKEN_DEDENT)) {
        error(parser, "Expect newline after 'del'.");
    }
}
```

#### 3. 虚拟机 (src/vm/vm.c)
添加 OP_DELETE 处理：
```c
case OP_DELETE: {
    // del 语句: 删除全局变量
    uint8_t name_index = READ_BYTE();
    if (name_index >= name_table_count) {
        runtime_error(vm, "Invalid variable name index.");
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    char* name = name_table_names[name_index];
    
    // 查找并删除全局变量
    ms_global_t* prev = NULL;
    ms_global_t* current = vm->globals;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // 找到了，删除它
            if (prev == NULL) {
                vm->globals = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->name);
            free(current);
            goto deleted_global;
        }
        prev = current;
        current = current->next;
    }
    
    runtime_error(vm, "Undefined variable '%s'.", name);
    return MS_RESULT_RUNTIME_ERROR;
    
deleted_global:
    break;
}
```

## 功能特性

### Assert 语句
- ✅ 基本断言
- ✅ 带错误消息的断言
- ✅ 断言失败时抛出 AssertionError
- ❌ 优化模式下禁用断言（未实现）

### Del 语句
- ✅ 删除全局变量
- ❌ 删除局部变量（不支持）
- ❌ 删除列表元素 `del list[index]`（未实现）
- ❌ 删除字典键 `del dict[key]`（未实现）

## 与 Python 3 的兼容性

### 完全兼容 ✅
```python
# Assert
assert x == 10
assert x > 5, "x must be greater than 5"

# Del
temp = 42
del temp
```

### 部分兼容 🚧
```python
# Del 只支持全局变量
def func():
    x = 10
    del x  # 不支持
```

### 不兼容 ❌
```python
# Del 列表元素和字典键
del list[0]  # 未实现
del dict['key']  # 未实现
```

## 测试文件

- ✅ test_assert_simple.ms - Assert 测试
- ✅ test_del_simple.ms - Del 测试
- ✅ test_del_debug.ms - Del 调试测试
- ✅ test_assert_del.ms - 综合测试

## 代码统计

### 新增代码
- parser.c: ~60 行（assert 和 del 处理）
- vm.c: ~60 行（OP_ASSERT 和 OP_DELETE 处理）
- lexer.c: ~5 行（修复 del 关键字识别）

### 总计
约 125 行新代码

## 实现时间

- Assert: 约 30 分钟
- Del: 约 1 小时（包括调试词法分析器）
- 总计: 约 1.5 小时

## 下一步

Assert 和 Del 已完成！接下来实现：

1. **for/while else 子句** - 预计 3-4 小时
2. **默认参数** - 预计 4-6 小时

## 总结

Assert 和 Del 语句的实现非常成功！它们都符合 Python 3 的基本语法，并且所有测试都通过了。

实现进度：
- ✅ Lambda 表达式
- ✅ Assert 语句
- ✅ Del 语句
- ⏳ For/while else 子句（下一个）

第一优先级完成度：3/9 (33%)
