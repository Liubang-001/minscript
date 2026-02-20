# 方法调用Bug修复

## 问题描述

当在native函数（如print）的参数中调用方法时，第二次调用会失败：

```python
print(t.method1())  # 成功
print(t.method2())  # 失败："Can only call functions"
```

但是以下方式工作正常：
```python
r1 = t.method1()
r2 = t.method2()
print(r1, r2)  # 成功
```

## 根本原因

问题出在`src/vm/vm.c`中的`OP_CALL`处理中，特别是绑定方法和类实例化时的栈管理。

### 栈布局问题

当调用绑定方法时：
1. 栈初始状态：`[bound_method, arg1, arg2, ...]`
2. 将bound_method替换为receiver：`[receiver, arg1, arg2, ...]`
3. 计算call_stack_base时错误地使用了：`stack_top - function->arity - 1`

问题在于：我们已经将bound_method替换为receiver，所以栈上只有`function->arity`个元素（receiver + 其他参数），而不是`function->arity + 1`个元素。

### 错误的计算

```c
// 错误的代码
ms_value_t* call_stack_base = vm->stack_top - function->arity - 1;
```

这会导致call_stack_base指向错误的位置，在函数返回后恢复栈时会破坏栈状态。

## 解决方案

修正call_stack_base的计算，使其正确指向receiver之前的位置：

```c
// 正确的代码
// 注意：栈上现在是 [receiver, arg1, arg2, ...]，没有bound_method
ms_value_t* call_stack_base = vm->stack_top - function->arity;
```

### 修改位置

1. **绑定方法调用**（第590行附近）：
   - 修改：`call_stack_base = vm->stack_top - function->arity`
   - 原因：栈上只有receiver和参数，没有bound_method

2. **类实例化__init__调用**（第665行附近）：
   - 修改：`call_stack_base = vm->stack_top - function->arity`
   - 原因：栈上只有instance和参数，没有class

3. **Frame指针刷新**：
   - 在每次递归调用`run()`后，添加：`frame = &vm->frames[vm->frame_count - 1];`
   - 原因：递归调用后，局部变量frame可能指向错误的位置

## 测试验证

所有以下测试现在都通过：

```python
# 测试1：直接在print中调用
class Test:
    def get(self):
        return 1

t = Test()
print(t.get())  # 成功
print(t.get())  # 成功

# 测试2：带参数的方法
class Calculator:
    def __init__(self, base):
        self.base = base
    
    def add(self, x):
        return self.base + x

calc = Calculator(10)
print(calc.add(5))   # 成功
print(calc.add(10))  # 成功

# 测试3：嵌套调用
print(calc.add(calc.add(5)))  # 成功
```

## 影响范围

这个修复解决了：
- ✅ 绑定方法在native函数参数中的调用
- ✅ 多次连续的方法调用
- ✅ 嵌套的方法调用
- ✅ 类实例化时__init__方法的调用

## 相关文件

- `src/vm/vm.c` - 主要修复文件
- `test_simple_twice.ms` - 简单测试
- `test_method_comprehensive.ms` - 综合测试
