# Python 3 语法特性实现路线图

## 概述
本文档详细说明如何 100% 实现 `examples/test.py` 中的所有 Python 3 语法特性。

## 当前实现状态

### ✅ 已完全实现（约 40%）
1. 基础数据类型（int, float, str, bool, None）
2. 列表、元组、字典
3. 所有算术运算符（+, -, *, /, //, %, **）
4. 所有比较运算符（==, !=, >, <, >=, <=）
5. 逻辑运算符（and, or, not）
6. if/elif/else 语句
7. for 循环（支持 range, 列表, 字符串等）
8. while 循环
9. break/continue 语句
10. pass 语句
11. 函数定义（def）和调用
12. return 语句
13. 切片语法（完整支持）
14. f-string 格式化
15. 26 个内置函数
16. 列表推导式（带三元表达式）

### 🚧 部分实现（约 10%）
1. 类定义（TOKEN_CLASS 已定义，但未实现）
2. lambda（TOKEN_LAMBDA 已定义，但未实现）
3. with 语句（TOKEN_WITH 已定义，但未实现）

### ❌ 未实现（约 50%）
需要实现的功能按优先级排序：

## 实现计划

### 阶段 1：基础语法增强（2-3天）

#### 1.1 lambda 表达式
**优先级：高**
**复杂度：中**

```python
# 语法
lambda 参数列表: 表达式

# 示例
add = lambda x, y: x + y
square = lambda x: x ** 2
```

**实现步骤：**
1. 词法分析器：已有 TOKEN_LAMBDA ✅
2. 解析器：
   - 在 `parse_precedence()` 中添加 lambda 处理
   - 解析参数列表
   - 解析表达式体
   - 生成 OP_LAMBDA 指令
3. 虚拟机：
   - 添加 OP_LAMBDA 操作码 ✅
   - 创建匿名函数对象
   - 捕获闭包变量
4. 测试：test_lambda.ms

**代码位置：**
- `src/parser/parser.c`: 添加 `lambda()` 函数
- `src/vm/vm.c`: 实现 OP_LAMBDA 处理

#### 1.2 assert 语句
**优先级：高**
**复杂度：低**

```python
assert 条件
assert 条件, "错误消息"
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_ASSERT ✅
2. 解析器：
   - 在 `statement()` 中添加 assert 处理
   - 解析条件表达式
   - 可选：解析错误消息
   - 生成 OP_ASSERT 指令
3. 虚拟机：
   - 添加 OP_ASSERT 操作码 ✅
   - 检查条件
   - 失败时抛出 AssertionError
4. 测试：test_assert.ms

#### 1.3 del 语句
**优先级：中**
**复杂度：中**

```python
del 变量
del 列表[索引]
del 字典[键]
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_DEL ✅
2. 解析器：
   - 在 `statement()` 中添加 del 处理
   - 解析目标（变量、索引、属性）
   - 生成 OP_DELETE 指令
3. 虚拟机：
   - 添加 OP_DELETE 操作码 ✅
   - 删除变量/元素/属性
4. 测试：test_del.ms

#### 1.4 for/while else 子句
**优先级：高**
**复杂度：中**

```python
for item in iterable:
    # 循环体
else:
    # 正常结束时执行（未 break）

while 条件:
    # 循环体
else:
    # 正常结束时执行（未 break）
```

**实现步骤：**
1. 词法分析器：已有 TOKEN_ELSE ✅
2. 解析器：
   - 修改 `for_statement()` 和 `while_statement()`
   - 检测 else 子句
   - 使用跳转指令实现逻辑
3. 虚拟机：
   - 修改 OP_FOR_END 和循环逻辑
   - 添加标志位跟踪是否 break
4. 测试：test_loop_else.ms

### 阶段 2：函数增强（2-3天）

#### 2.1 默认参数
**优先级：高**
**复杂度：中**

```python
def greet(name, greeting="Hello"):
    return f"{greeting}, {name}"
```

**实现步骤：**
1. 解析器：
   - 修改 `function_declaration()`
   - 解析默认值表达式
   - 存储默认值
2. 虚拟机：
   - 修改函数对象结构
   - 调用时处理缺失参数
3. 测试：test_default_args.ms

#### 2.2 关键字参数
**优先级：中**
**复杂度：高**

```python
def func(a, b, *, c, d=10):
    pass

func(1, 2, c=3, d=4)
```

**实现步骤：**
1. 解析器：
   - 解析 * 分隔符
   - 解析关键字参数
2. 虚拟机：
   - 修改调用约定
   - 参数匹配逻辑
3. 测试：test_kwargs.ms

#### 2.3 可变参数 (*args, **kwargs)
**优先级：中**
**复杂度：高**

```python
def func(*args, **kwargs):
    pass
```

**实现步骤：**
1. 解析器：
   - 识别 * 和 ** 前缀
   - 生成特殊参数标记
2. 虚拟机：
   - 收集额外参数到元组/字典
3. 测试：test_varargs.ms

### 阶段 3：面向对象（4-5天）

#### 3.1 类定义基础
**优先级：高**
**复杂度：高**

```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"I'm {self.name}"
```

**实现步骤：**
1. 词法分析器：已有 TOKEN_CLASS ✅
2. 解析器：
   - 添加 `class_declaration()`
   - 解析类名、方法
   - 生成 OP_CLASS 指令
3. 虚拟机：
   - 添加类对象类型
   - 添加实例对象类型
   - 实现方法查找
4. 测试：test_class_basic.ms

#### 3.2 继承和 super()
**优先级：高**
**复杂度：高**

```python
class Student(Person):
    def __init__(self, name, age, student_id):
        super().__init__(name, age)
        self.student_id = student_id
```

**实现步骤：**
1. 解析器：
   - 解析父类
   - 解析 super() 调用
2. 虚拟机：
   - 实现继承链
   - 实现方法解析顺序（MRO）
3. 测试：test_inheritance.ms

#### 3.3 魔术方法
**优先级：中**
**复杂度：中**

```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __str__(self):
        return f"Point({self.x}, {self.y})"
    
    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)
```

**实现步骤：**
1. 虚拟机：
   - 在运算符处理中查找魔术方法
   - __init__, __str__, __repr__
   - __add__, __sub__, __mul__ 等
2. 测试：test_magic_methods.ms

#### 3.4 装饰器（@property, @classmethod, @staticmethod）
**优先级：中**
**复杂度：高**

```python
class Person:
    @property
    def age(self):
        return self._age
    
    @classmethod
    def from_birth_year(cls, name, year):
        pass
    
    @staticmethod
    def is_adult(age):
        pass
```

**实现步骤：**
1. 解析器：
   - 解析 @ 装饰器语法
   - 应用装饰器到方法
2. 虚拟机：
   - 实现 property 对象
   - 实现 classmethod/staticmethod 包装
3. 测试：test_decorators.ms

### 阶段 4：异常处理（3-4天）

#### 4.1 try/except/else/finally
**优先级：高**
**复杂度：高**

```python
try:
    # 可能出错的代码
    x = 10 / 0
except ZeroDivisionError:
    # 处理异常
    print("除零错误")
except ValueError as e:
    # 捕获异常对象
    print(f"值错误: {e}")
else:
    # 没有异常时执行
    print("成功")
finally:
    # 总是执行
    print("清理")
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_TRY, TOKEN_EXCEPT, TOKEN_FINALLY ✅
2. 解析器：
   - 添加 `try_statement()`
   - 解析 try/except/else/finally 块
   - 生成异常处理指令
3. 虚拟机：
   - 添加异常栈
   - 实现异常传播
   - 实现 finally 保证执行
4. 测试：test_exceptions.ms

#### 4.2 raise 语句
**优先级：高**
**复杂度：中**

```python
raise ValueError("Invalid value")
raise  # 重新抛出当前异常
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_RAISE ✅
2. 解析器：
   - 解析 raise 语句
   - 生成 OP_RAISE 指令
3. 虚拟机：
   - 实现异常抛出
4. 测试：test_raise.ms

#### 4.3 自定义异常类
**优先级：中**
**复杂度：中**

```python
class MyError(Exception):
    def __init__(self, message, code):
        self.message = message
        self.code = code
```

**实现步骤：**
1. 依赖：需要先实现类系统
2. 虚拟机：
   - 实现 Exception 基类
   - 支持异常继承
3. 测试：test_custom_exceptions.ms

### 阶段 5：高级特性（3-4天）

#### 5.1 上下文管理器（with）
**优先级：中**
**复杂度：高**

```python
with open("file.txt") as f:
    content = f.read()

class MyContext:
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        return False
```

**实现步骤：**
1. 词法分析器：已有 TOKEN_WITH ✅
2. 解析器：
   - 添加 `with_statement()`（已有，需完善）
   - 解析 as 子句
3. 虚拟机：
   - 调用 __enter__ 和 __exit__
   - 处理异常传递
4. 测试：test_with.ms

#### 5.2 生成器（yield）
**优先级：中**
**复杂度：高**

```python
def count_up_to(n):
    i = 1
    while i <= n:
        yield i
        i += 1

for num in count_up_to(5):
    print(num)
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_YIELD ✅
2. 解析器：
   - 识别 yield 语句
   - 标记函数为生成器
3. 虚拟机：
   - 实现生成器对象
   - 实现状态保存/恢复
   - 实现 __iter__ 和 __next__
4. 测试：test_generators.ms

#### 5.3 迭代器协议
**优先级：中**
**复杂度：中**

```python
class CountDown:
    def __init__(self, start):
        self.current = start
    
    def __iter__(self):
        return self
    
    def __next__(self):
        if self.current <= 0:
            raise StopIteration
        value = self.current
        self.current -= 1
        return value
```

**实现步骤：**
1. 依赖：需要先实现类系统和异常
2. 虚拟机：
   - 在 for 循环中调用 __iter__ 和 __next__
   - 处理 StopIteration
3. 测试：test_iterators.ms

#### 5.4 global/nonlocal
**优先级：低**
**复杂度：中**

```python
x = 10

def outer():
    x = 20
    def inner():
        nonlocal x
        x += 5
    inner()

def change_global():
    global x
    x += 100
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_GLOBAL, TOKEN_NONLOCAL ✅
2. 解析器：
   - 解析 global/nonlocal 声明
   - 修改变量解析逻辑
3. 虚拟机：
   - 修改变量查找规则
4. 测试：test_scopes.ms

### 阶段 6：异步编程（4-5天）

#### 6.1 async/await
**优先级：低**
**复杂度：非常高**

```python
async def fetch_data(delay, name):
    await asyncio.sleep(delay)
    return f"{name} data"

async def main():
    result = await fetch_data(1, "test")
    print(result)
```

**实现步骤：**
1. 词法分析器：添加 TOKEN_ASYNC, TOKEN_AWAIT ✅
2. 解析器：
   - 解析 async def
   - 解析 await 表达式
3. 虚拟机：
   - 实现协程对象
   - 实现事件循环
   - 实现 await 机制
4. 测试：test_async.ms

#### 6.2 异步上下文管理器
**优先级：低**
**复杂度：高**

```python
class AsyncResource:
    async def __aenter__(self):
        return self
    
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        pass

async def main():
    async with AsyncResource() as res:
        pass
```

**实现步骤：**
1. 依赖：需要先实现 async/await
2. 解析器：
   - 解析 async with
3. 虚拟机：
   - 调用 __aenter__ 和 __aexit__
4. 测试：test_async_with.ms

#### 6.3 异步生成器
**优先级：低**
**复杂度：高**

```python
async def async_counter(n):
    for i in range(n):
        await asyncio.sleep(0.1)
        yield i

async def main():
    async for num in async_counter(5):
        print(num)
```

**实现步骤：**
1. 依赖：需要先实现 async/await 和生成器
2. 解析器：
   - 解析 async for
3. 虚拟机：
   - 实现异步迭代协议
4. 测试：test_async_generators.ms

### 阶段 7：其他功能（1-2天）

#### 7.1 海象运算符 (:=)
**优先级：低**
**复杂度：中**

```python
if (n := len(text)) > 10:
    print(f"长度 {n} 大于 10")
```

**实现步骤：**
1. 词法分析器：
   - 添加 TOKEN_WALRUS (:=)
2. 解析器：
   - 在表达式中处理赋值
3. 虚拟机：
   - 赋值并返回值
4. 测试：test_walrus.ms

#### 7.2 eval/exec
**优先级：低**
**复杂度：高**

```python
code = "print('Hello')"
eval(code)
exec("x = 5; print(x)")
```

**实现步骤：**
1. 内置函数：
   - 添加 builtin_eval()
   - 添加 builtin_exec()
2. 虚拟机：
   - 动态编译和执行代码
3. 测试：test_eval_exec.ms

## 实现优先级总结

### 第一批（核心功能，2周）
1. ✅ lambda 表达式
2. ✅ assert 语句
3. ✅ del 语句
4. ✅ for/while else 子句
5. ✅ 默认参数
6. ✅ 类定义基础
7. ✅ 继承和 super()
8. ✅ try/except/finally
9. ✅ raise 语句

### 第二批（增强功能，1-2周）
10. 关键字参数
11. *args/**kwargs
12. 魔术方法
13. @property/@classmethod/@staticmethod
14. 上下文管理器
15. 生成器
16. 迭代器协议

### 第三批（高级功能，1-2周）
17. global/nonlocal
18. 海象运算符
19. 自定义异常类
20. eval/exec

### 第四批（异步功能，可选，2-3周）
21. async/await
22. 异步上下文管理器
23. 异步生成器

## 测试策略

每个功能实现后，创建对应的测试文件：
- test_lambda.ms
- test_assert.ms
- test_del.ms
- test_loop_else.ms
- test_default_args.ms
- test_class_basic.ms
- test_inheritance.ms
- test_exceptions.ms
- test_raise.ms
- test_with.ms
- test_generators.ms
- test_iterators.ms
- test_scopes.ms
- test_async.ms

最终目标：
```bash
# 将 test.py 重命名为 test_python_full.ms
cp examples/test.py examples/test_python_full.ms
# 直接运行
./miniscript.exe examples/test_python_full.ms
```

## 时间估算

- 第一批：2周（10个工作日）
- 第二批：1-2周（5-10个工作日）
- 第三批：1-2周（5-10个工作日）
- 第四批：2-3周（10-15个工作日，可选）

总计：4-7周（不含异步功能）或 6-10周（含异步功能）

## 成功标准

1. ✅ examples/test.py 中的所有代码都能正确执行
2. ✅ 输出结果与 Python 3 一致
3. ✅ 所有功能都有对应的测试用例
4. ✅ 代码覆盖率达到 90% 以上
5. ✅ 文档完整，包含所有新功能的说明

## 下一步行动

1. 从 lambda 表达式开始实现
2. 每完成一个功能，立即编写测试
3. 更新 IMPLEMENTATION_STATUS.md
4. 提交代码并标记版本号
