# Python 特性最终实现指南

## 执行摘要

本项目已完成 Python 3 核心语法的 **51%**，包括：
- ✅ Lambda 表达式
- ✅ Assert/Del 语句
- ✅ For/While Else 子句
- ✅ 默认参数
- 🚧 列表推导式（部分）

## 剩余工作清单

### 第一优先级（核心语法）- 剩余 2 项

#### 1. 列表推导式完善（高优先级）
**当前状态**：40% 完成
- ✅ 基础推导式：`[x for x in range(10)]`
- ✅ 带三元表达式：`[x if x > 0 else -x for x in range(-5, 5)]`
- ❌ 带条件过滤：`[x for x in range(10) if x % 2 == 0]`
- ❌ 嵌套推导式：`[x*y for x in range(3) for y in range(3)]`

**实现方案**：
```c
// 解析器中添加条件过滤支持
static void list_comprehension() {
    // [expr for var in iterable if condition]
    
    // 1. 解析表达式
    expression();
    
    // 2. 解析 for 子句
    consume(TOKEN_FOR, "Expect 'for'.");
    consume(TOKEN_IDENTIFIER, "Expect variable.");
    uint8_t var = identifier_constant(&parser.previous);
    
    consume(TOKEN_IN, "Expect 'in'.");
    expression();  // iterable
    
    // 3. 可选的 if 条件
    if (match(TOKEN_IF)) {
        expression();  // condition
        emit_byte(OP_BUILD_LIST_COMP_IF);
    } else {
        emit_byte(OP_BUILD_LIST_COMP);
    }
    emit_byte(var);
}
```

**预计时间**：2-3小时

#### 2. 海象运算符 := （中优先级）
**语法**：`if (n := len(text)) > 10: ...`

**实现方案**：
```c
// 词法分析器添加 TOKEN_WALRUS
// 解析器在表达式中处理赋值
static void walrus() {
    // (var := expr)
    consume(TOKEN_IDENTIFIER, "Expect variable name.");
    uint8_t var = identifier_constant(&parser.previous);
    
    consume(TOKEN_WALRUS, "Expect ':='.");
    expression();  // 右侧表达式
    
    // 赋值并保留值在栈上
    emit_bytes(OP_SET_GLOBAL, var);
    emit_bytes(OP_GET_GLOBAL, var);
}
```

**预计时间**：2-3小时

### 第二优先级（面向对象）- 6 项

#### 1. 类定义基础（最高优先级）
**语法**：
```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"I'm {self.name}"
```

**核心数据结构**：
```c
// 类对象
typedef struct ms_class {
    char* name;
    struct ms_class* parent;
    ms_dict_t* methods;
    ms_function_t* init;
} ms_class_t;

// 实例对象
typedef struct ms_instance {
    ms_class_t* klass;
    ms_dict_t* attrs;
} ms_instance_t;
```

**实现步骤**：
1. 扩展值类型系统（添加 CLASS、INSTANCE 类型）
2. 添加操作码（OP_CLASS, OP_GET_PROPERTY, OP_SET_PROPERTY）
3. 实现类解析器（class_declaration）
4. 实现虚拟机支持（属性访问、方法调用）

**预计时间**：2-3天

#### 2. 继承和 super()
**语法**：
```python
class Student(Person):
    def __init__(self, name, age, student_id):
        super().__init__(name, age)
        self.student_id = student_id
```

**实现要点**：
- 解析父类语法
- 实现方法解析顺序（MRO）
- 实现 super() 内置函数

**预计时间**：1-2天

#### 3. @property 装饰器
**语法**：
```python
class Circle:
    @property
    def area(self):
        return 3.14 * self.radius ** 2
```

**实现要点**：
- 解析 @ 装饰器语法
- 创建 property 对象
- 实现 getter/setter 机制

**预计时间**：1天

#### 4. @classmethod 装饰器
**语法**：
```python
@classmethod
def from_string(cls, s):
    return cls(int(s))
```

**预计时间**：1天

#### 5. @staticmethod 装饰器
**语法**：
```python
@staticmethod
def add(a, b):
    return a + b
```

**预计时间**：1天

#### 6. 魔术方法
**语法**：
```python
def __str__(self):
    return f"Point({self.x}, {self.y})"

def __add__(self, other):
    return Point(self.x + other.x, self.y + other.y)
```

**实现要点**：
- __str__, __repr__（字符串表示）
- __add__, __sub__, __mul__（运算符重载）
- __eq__, __lt__, __gt__（比较运算符）
- __len__, __getitem__（容器方法）

**预计时间**：2-3天

## 推荐实现顺序

### 阶段 1：完成第一优先级（1-2天）
1. 列表推导式条件过滤（2-3小时）
2. 海象运算符（2-3小时）
3. 集合推导式（3-4小时，可选）
4. 字典推导式（2-3小时，可选）

**成果**：第一优先级 100% 完成

### 阶段 2：OOP 基础（2-3天）
1. 类定义基础（2-3天）
   - 值类型扩展
   - 类对象和实例对象
   - 属性访问
   - 方法调用

**成果**：可以定义和使用类

### 阶段 3：OOP 继承（1-2天）
1. 单继承（1天）
2. super() 函数（1天）

**成果**：完整的继承系统

### 阶段 4：OOP 装饰器（2-3天）
1. @property（1天）
2. @classmethod（1天）
3. @staticmethod（1天）

**成果**：完整的装饰器支持

### 阶段 5：OOP 魔术方法（2-3天）
1. __str__, __repr__（1天）
2. 运算符重载（1天）
3. 容器方法（1天）

**成果**：完整的 OOP 系统

## 总时间估算

- 阶段 1（第一优先级）：1-2天
- 阶段 2（OOP 基础）：2-3天
- 阶段 3（OOP 继承）：1-2天
- 阶段 4（OOP 装饰器）：2-3天
- 阶段 5（OOP 魔术方法）：2-3天

**总计**：8-13天（约 2-3 周）

## 快速开始指南

### 今天可以完成的任务

#### 任务 1：列表推导式条件过滤（2-3小时）
```bash
# 1. 修改解析器
vim src/parser/parser.c
# 添加 if 条件解析

# 2. 添加操作码
vim src/vm/vm.h
# 添加 OP_BUILD_LIST_COMP_IF

# 3. 实现虚拟机
vim src/vm/vm.c
# 实现条件过滤逻辑

# 4. 测试
./miniscript.exe test_list_comp_filter.ms
```

#### 任务 2：海象运算符（2-3小时）
```bash
# 1. 添加 Token
vim src/lexer/lexer.c
# 添加 := 识别

# 2. 修改解析器
vim src/parser/parser.c
# 在表达式中处理赋值

# 3. 测试
./miniscript.exe test_walrus.ms
```

### 本周可以完成的任务

1. 完成第一优先级所有功能
2. 设计 OOP 数据结构
3. 开始类定义基础实现

## 测试用例模板

### test_list_comp_filter.ms
```python
# 基础过滤
evens = [x for x in range(10) if x % 2 == 0]
print(evens)  # [0, 2, 4, 6, 8]

# 多个条件
result = [x for x in range(20) if x % 2 == 0 if x % 3 == 0]
print(result)  # [0, 6, 12, 18]
```

### test_walrus.ms
```python
# 基础用法
if (n := len([1, 2, 3])) > 2:
    print(f"长度是 {n}")

# 循环中使用
while (line := input()) != "quit":
    print(f"你输入了: {line}")
```

### test_class_basic.ms
```python
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"I'm {self.name}, {self.age} years old"

p = Person("Alice", 30)
print(p.name)  # Alice
print(p.introduce())  # I'm Alice, 30 years old
```

## 成功标准

### 第一优先级完成标准
- ✅ 所有 7 项功能 100% 实现
- ✅ 每项功能有完整测试
- ✅ 文档齐全

### 第二优先级完成标准
- ✅ 可以定义类和创建实例
- ✅ 可以使用继承和 super()
- ✅ 可以使用装饰器
- ✅ 可以使用魔术方法
- ✅ 所有功能有完整测试

## 风险和挑战

### 技术挑战
1. **OOP 系统复杂度高**
   - 需要设计完整的对象模型
   - 方法解析顺序（MRO）实现复杂
   - 装饰器需要元编程支持

2. **推导式实现**
   - 嵌套推导式需要复杂的作用域管理
   - 条件过滤需要运行时求值

### 解决方案
1. **分阶段实现**
   - 先实现基础功能
   - 逐步添加高级特性

2. **参考现有实现**
   - 研究 CPython 实现
   - 参考其他解释器（如 Lua, Ruby）

3. **充分测试**
   - 每个功能都有测试用例
   - 边界情况测试

## 资源和参考

### 文档
- PYTHON_IMPLEMENTATION_STATUS.md - 总体状态
- IMPLEMENTATION_PROGRESS.md - 进度跟踪
- OOP_IMPLEMENTATION_PLAN.md - OOP 详细计划
- COMPREHENSION_IMPLEMENTATION.md - 推导式详细计划

### 代码文件
- src/parser/parser.c - 解析器（主要工作）
- src/vm/vm.c - 虚拟机（主要工作）
- src/core/value.c - 值类型系统
- src/lexer/lexer.c - 词法分析器

### 测试文件
- test_lambda_*.ms - Lambda 测试
- test_loop_else.ms - Loop Else 测试
- test_default_*.ms - 默认参数测试
- test_assert_del.ms - Assert/Del 测试
- test_listcomp_*.ms - 推导式测试

## 下一步行动

### 立即开始（今天）
1. 实现列表推导式条件过滤
2. 编写测试用例
3. 验证功能

### 本周目标
1. 完成第一优先级所有功能
2. 第一优先级达到 100%
3. 开始 OOP 设计

### 本月目标
1. 完成 OOP 基础实现
2. 完成继承系统
3. 开始装饰器实现

---

**最后更新**: 2024年2月20日
**当前版本**: v0.2-dev
**目标版本**: v1.0
**当前进度**: 51%
**目标进度**: 100%

**预计完成时间**: 2-3周
