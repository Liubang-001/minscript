# Python 3 语法特性实现计划

## 目标
100% 实现 examples/test.py 中的所有 Python 3 语法特性

## 当前状态分析

### 已实现 ✅
1. 基础数据类型（int, float, str, bool, None）
2. 列表、元组、字典
3. 基础运算符（+, -, *, /, //, %, **）
4. 比较运算符（==, !=, >, <, >=, <=）
5. 逻辑运算符（and, or, not）
6. if/elif/else
7. for/while 循环
8. break/continue
9. 函数定义（def）
10. 切片语法
11. f-string 格式化
12. 26 个内置函数

### 需要实现 ❌

#### 第一优先级（核心语法）
1. **列表/集合/字典推导式** - 高频使用
2. **lambda 表达式** - 函数式编程基础
3. **for/while 的 else 子句** - Python 特色
4. **默认参数** - 函数增强
5. **海象运算符 :=** - Python 3.8+
6. **assert 语句** - 调试工具
7. **del 语句** - 内存管理

#### 第二优先级（面向对象）
8. **类定义（class）** - OOP 基础
9. **继承（super）** - OOP 核心
10. **属性装饰器（@property）** - 封装
11. **类方法（@classmethod）** - 工厂模式
12. **静态方法（@staticmethod）** - 工具方法
13. **魔术方法（__init__, __str__, __repr__, __add__ 等）** - 运算符重载

#### 第三优先级（异常处理）
14. **try/except/else/finally** - 异常捕获
15. **raise** - 异常抛出
16. **自定义异常类** - 异常扩展

#### 第四优先级（高级特性）
17. **上下文管理器（with, __enter__, __exit__）** - 资源管理
18. **装饰器（@decorator）** - 元编程
19. **生成器（yield）** - 惰性求值
20. **迭代器协议（__iter__, __next__）** - 自定义迭代
21. ***args 和 **kwargs** - 可变参数
22. **关键字参数（*）** - 参数约束
23. **global/nonlocal** - 作用域控制

#### 第五优先级（异步编程）
24. **async/await** - 异步函数
25. **异步上下文管理器（async with）** - 异步资源
26. **异步生成器（async for, yield）** - 异步迭代

#### 第六优先级（其他）
27. **eval/exec** - 动态执行
28. **类型注解（运行时支持）** - 类型检查

## 实现策略

### 阶段 1：推导式和 lambda（1-2天）
- 列表推导式（已部分实现，需完善）
- 集合推导式
- 字典推导式
- lambda 表达式
- 生成器表达式

### 阶段 2：函数增强（1天）
- 默认参数
- 关键字参数
- *args 和 **kwargs
- for/while else 子句

### 阶段 3：面向对象基础（2-3天）
- class 定义
- __init__ 构造函数
- 实例属性和方法
- 继承和 super()
- 魔术方法（__str__, __repr__, __add__ 等）

### 阶段 4：装饰器和属性（1-2天）
- @property
- @classmethod
- @staticmethod
- 自定义装饰器
- 类装饰器

### 阶段 5：异常处理（1-2天）
- try/except/else/finally
- raise
- 自定义异常类
- 异常链

### 阶段 6：高级特性（2-3天）
- 上下文管理器（with）
- 生成器（yield）
- 迭代器协议
- global/nonlocal

### 阶段 7：异步编程（3-4天）
- async/await
- 异步上下文管理器
- 异步生成器
- asyncio 基础

### 阶段 8：其他语法（1天）
- 海象运算符 :=
- assert
- del
- eval/exec

## 技术实现要点

### 词法分析器扩展
- 添加新关键字：class, lambda, try, except, finally, raise, with, yield, async, await, global, nonlocal, assert, del
- 添加新运算符：:=（海象运算符）

### 解析器扩展
- 推导式语法解析
- lambda 表达式解析
- 类定义解析
- 异常处理语法解析
- 装饰器语法解析
- 生成器语法解析

### 虚拟机扩展
- 新增操作码：
  - OP_BUILD_SET, OP_BUILD_DICT_COMP
  - OP_LAMBDA
  - OP_CLASS, OP_INHERIT, OP_GET_SUPER
  - OP_TRY_BEGIN, OP_TRY_END, OP_CATCH, OP_THROW
  - OP_ENTER_CONTEXT, OP_EXIT_CONTEXT
  - OP_YIELD, OP_RESUME
  - OP_GLOBAL, OP_NONLOCAL
  - OP_ASSERT, OP_DELETE

### 运行时支持
- 类对象和实例对象
- 异常对象和异常栈
- 上下文管理器协议
- 生成器状态机
- 装饰器链

## 测试策略

每个功能实现后，创建对应的测试文件：
- test_comprehensions.ms
- test_lambda.ms
- test_class.ms
- test_exceptions.ms
- test_decorators.ms
- test_generators.ms
- test_async.ms
- test_misc.ms

最终目标：examples/test.py 可以直接作为 .ms 文件运行！

## 时间估算
- 总计：15-20 天
- 每天 4-6 小时开发时间
- 包含测试和文档编写

## 成功标准
1. examples/test.py 中的所有代码都能正确执行
2. 输出结果与 Python 3 一致
3. 所有功能都有对应的测试用例
4. 代码覆盖率达到 90% 以上
