# 立即实现计划

## 实现顺序

### 阶段 1：完成第一优先级（今天，4-6小时）

#### 1. 列表推导式条件过滤（2小时）
**状态**: 🚧 40% → 100%

**需要实现**:
```python
# 带 if 条件
[x for x in range(10) if x % 2 == 0]  # [0, 2, 4, 6, 8]

# 多个 if 条件
[x for x in range(20) if x % 2 == 0 if x % 3 == 0]  # [0, 6, 12, 18]
```

**实现步骤**:
1. 修改 `src/parser/parser.c` - 在列表推导式解析中添加 if 条件支持
2. 当前实现已经有基础框架，需要完善条件过滤逻辑
3. 测试文件: `test_listcomp_filter.ms`

#### 2. 海象运算符 := （2小时）
**状态**: ❌ 0% → 100%

**语法**:
```python
if (n := len([1, 2, 3])) > 2:
    print(f"长度是 {n}")

while (line := input()) != "quit":
    print(f"你输入了: {line}")
```

**实现步骤**:
1. 修改 `src/lexer/lexer.c` - 添加 `:=` token 识别
2. 修改 `src/parser/parser.c` - 在表达式中处理赋值
3. 添加 `OP_WALRUS` 操作码
4. 测试文件: `test_walrus.ms`

### 阶段 2：OOP 基础（2-3天）

#### 3. 类定义基础（2-3天）
**状态**: ❌ 0% → 100%

**语法**:
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

**核心数据结构**:
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

// 绑定方法
typedef struct ms_bound_method {
    ms_instance_t* receiver;
    ms_function_t* method;
} ms_bound_method_t;
```

**实现步骤**:
1. 在 `include/miniscript.h` 中添加类型定义
2. 在 `src/core/value.c` 中实现类和实例操作
3. 在 `src/parser/parser.c` 中添加 `class_declaration()`
4. 在 `src/vm/vm.c` 中实现类相关操作码
5. 测试文件: `test_class_basic.ms`

**新增操作码**:
```c
OP_CLASS,           // 创建类
OP_GET_PROPERTY,    // 获取属性
OP_SET_PROPERTY,    // 设置属性
OP_METHOD,          // 定义方法
OP_INVOKE,          // 调用方法
```

#### 4. 继承和 super()（1-2天）
**状态**: ❌ 0% → 100%

**语法**:
```python
class Student(Person):
    def __init__(self, name, age, student_id):
        super().__init__(name, age)
        self.student_id = student_id
    
    def introduce(self):
        return f"Student: {self.name}, ID: {self.student_id}"

s = Student("Bob", 20, "S001")
print(s.introduce())
```

**实现步骤**:
1. 修改类定义解析，支持父类语法
2. 实现方法解析顺序（MRO）
3. 实现 `super()` 内置函数
4. 添加 `OP_INHERIT`, `OP_GET_SUPER` 操作码
5. 测试文件: `test_inheritance.ms`

#### 5. 装饰器（2-3天）

##### 5.1 @property
```python
class Circle:
    def __init__(self, radius):
        self._radius = radius
    
    @property
    def radius(self):
        return self._radius
    
    @radius.setter
    def radius(self, value):
        self._radius = value
    
    @property
    def area(self):
        return 3.14159 * self._radius ** 2
```

##### 5.2 @classmethod
```python
class Person:
    @classmethod
    def from_birth_year(cls, name, year):
        return cls(name, 2024 - year)
```

##### 5.3 @staticmethod
```python
class Math:
    @staticmethod
    def add(a, b):
        return a + b
```

**实现步骤**:
1. 解析 `@` 装饰器语法
2. 创建 property、classmethod、staticmethod 对象
3. 添加相应的值类型
4. 测试文件: `test_decorators.ms`

#### 6. 魔术方法（2-3天）

**常用魔术方法**:
```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __str__(self):
        return f"Point({self.x}, {self.y})"
    
    def __repr__(self):
        return f"Point({self.x}, {self.y})"
    
    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other):
        return Point(self.x - other.x, self.y - other.y)
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __len__(self):
        return 2
    
    def __getitem__(self, index):
        return self.x if index == 0 else self.y
```

**实现步骤**:
1. 在运算符处理中查找魔术方法
2. 修改 `OP_ADD`, `OP_PRINT` 等操作码
3. 测试文件: `test_magic_methods.ms`

## 详细实现代码

### 1. 列表推导式条件过滤

当前代码已经有基础实现，需要完善。查看 `src/parser/parser.c` 中的列表推导式部分。

### 2. 海象运算符实现

#### 2.1 词法分析器（src/lexer/lexer.c）
```c
// 在 scan_token() 中添加
case ':':
    if (peek() == '=') {
        advance();
        return make_token(TOKEN_WALRUS);
    }
    return make_token(TOKEN_COLON);
```

#### 2.2 添加 Token（src/lexer/lexer.h）
```c
TOKEN_WALRUS,  // :=
```

#### 2.3 解析器（src/parser/parser.c）
```c
// 在 grouping() 或 expression() 中处理
static void walrus_expression() {
    // (var := expr)
    if (check(TOKEN_IDENTIFIER)) {
        Token name = parser.current;
        advance();
        
        if (match(TOKEN_WALRUS)) {
            // 解析右侧表达式
            expression();
            
            // 赋值并保留值在栈上
            uint8_t global = identifier_constant(&name);
            emit_bytes(OP_SET_GLOBAL, global);
            emit_bytes(OP_GET_GLOBAL, global);
        }
    }
}
```

### 3. 类定义实现

#### 3.1 添加类型定义（include/miniscript.h）
```c
// 在值类型枚举中已有 MS_VAL_CLASS 和 MS_VAL_INSTANCE

// 添加新的结构体定义
typedef struct ms_class {
    char* name;
    struct ms_class* parent;
    ms_dict_t* methods;
} ms_class_t;

typedef struct ms_instance {
    ms_class_t* klass;
    ms_dict_t* attrs;
} ms_instance_t;

typedef struct ms_bound_method {
    ms_instance_t* receiver;
    struct ms_function* method;
} ms_bound_method_t;
```

#### 3.2 值操作函数（src/core/value.c）
```c
ms_value_t ms_value_class(ms_class_t* klass) {
    ms_value_t value;
    value.type = MS_VAL_CLASS;
    value.as.object = (ms_object_t*)klass;
    return value;
}

ms_value_t ms_value_instance(ms_instance_t* instance) {
    ms_value_t value;
    value.type = MS_VAL_INSTANCE;
    value.as.object = (ms_object_t*)instance;
    return value;
}

bool ms_value_is_class(ms_value_t value) {
    return value.type == MS_VAL_CLASS;
}

bool ms_value_is_instance(ms_value_t value) {
    return value.type == MS_VAL_INSTANCE;
}

ms_class_t* ms_value_as_class(ms_value_t value) {
    return (ms_class_t*)value.as.object;
}

ms_instance_t* ms_value_as_instance(ms_value_t value) {
    return (ms_instance_t*)value.as.object;
}
```

#### 3.3 类对象操作（src/core/class.c - 新文件）
```c
#include "class.h"
#include <stdlib.h>
#include <string.h>

ms_class_t* ms_class_new(const char* name) {
    ms_class_t* klass = malloc(sizeof(ms_class_t));
    klass->name = strdup(name);
    klass->parent = NULL;
    klass->methods = ms_dict_new();
    return klass;
}

void ms_class_free(ms_class_t* klass) {
    free(klass->name);
    ms_dict_free(klass->methods);
    free(klass);
}

ms_instance_t* ms_instance_new(ms_class_t* klass) {
    ms_instance_t* instance = malloc(sizeof(ms_instance_t));
    instance->klass = klass;
    instance->attrs = ms_dict_new();
    return instance;
}

void ms_instance_free(ms_instance_t* instance) {
    ms_dict_free(instance->attrs);
    free(instance);
}
```

#### 3.4 解析器（src/parser/parser.c）
```c
static void class_declaration() {
    // class ClassName:
    consume(TOKEN_IDENTIFIER, "Expect class name.");
    uint8_t name_constant = identifier_constant(&parser.previous);
    Token class_name = parser.previous;
    
    // 检查是否有父类
    if (match(TOKEN_LEFT_PAREN)) {
        consume(TOKEN_IDENTIFIER, "Expect superclass name.");
        variable(false);  // 加载父类
        
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after superclass.");
        emit_byte(OP_INHERIT);
    }
    
    // 创建类
    emit_bytes(OP_CLASS, name_constant);
    define_variable(name_constant);
    
    // 解析类体
    consume(TOKEN_COLON, "Expect ':' after class name.");
    consume(TOKEN_NEWLINE, "Expect newline.");
    consume(TOKEN_INDENT, "Expect indent.");
    
    // 加载类到栈顶（用于添加方法）
    named_variable(class_name, false);
    
    // 解析方法
    while (!check(TOKEN_DEDENT) && !check(TOKEN_EOF)) {
        if (match(TOKEN_DEF)) {
            method();
        } else if (match(TOKEN_AT)) {
            decorator();
        } else {
            error("Expect method definition in class body.");
        }
    }
    
    consume(TOKEN_DEDENT, "Expect dedent after class body.");
    emit_byte(OP_POP);  // 弹出类
}

static void method() {
    consume(TOKEN_IDENTIFIER, "Expect method name.");
    uint8_t constant = identifier_constant(&parser.previous);
    
    // 解析方法体（类似函数）
    function_type(TYPE_METHOD);
    
    // 添加方法到类
    emit_bytes(OP_METHOD, constant);
}
```

#### 3.5 虚拟机（src/vm/vm.c）
```c
case OP_CLASS: {
    const char* name = READ_STRING();
    ms_class_t* klass = ms_class_new(name);
    push(ms_value_class(klass));
    break;
}

case OP_METHOD: {
    const char* name = READ_STRING();
    ms_value_t method = peek(0);
    ms_class_t* klass = ms_value_as_class(peek(1));
    
    ms_dict_set(klass->methods, name, method);
    pop();  // 弹出方法
    break;
}

case OP_GET_PROPERTY: {
    if (!ms_value_is_instance(peek(0))) {
        runtime_error("Only instances have properties.");
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    ms_instance_t* instance = ms_value_as_instance(peek(0));
    const char* name = READ_STRING();
    
    // 先查找实例属性
    ms_value_t value;
    if (ms_dict_get(instance->attrs, name, &value)) {
        pop();  // instance
        push(value);
        break;
    }
    
    // 查找方法
    if (ms_dict_get(instance->klass->methods, name, &value)) {
        // 创建绑定方法
        ms_bound_method_t* bound = malloc(sizeof(ms_bound_method_t));
        bound->receiver = instance;
        bound->method = ms_value_as_function(value);
        
        pop();  // instance
        push(ms_value_bound_method(bound));
        break;
    }
    
    runtime_error("Undefined property '%s'.", name);
    return MS_RESULT_RUNTIME_ERROR;
}

case OP_SET_PROPERTY: {
    if (!ms_value_is_instance(peek(1))) {
        runtime_error("Only instances have properties.");
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    ms_instance_t* instance = ms_value_as_instance(peek(1));
    const char* name = READ_STRING();
    ms_value_t value = peek(0);
    
    ms_dict_set(instance->attrs, name, value);
    
    pop();  // value
    pop();  // instance
    push(value);
    break;
}

case OP_INVOKE: {
    // 调用方法（优化版本，直接调用而不创建绑定方法）
    const char* method_name = READ_STRING();
    int arg_count = READ_BYTE();
    
    ms_instance_t* instance = ms_value_as_instance(peek(arg_count));
    
    ms_value_t method;
    if (!ms_dict_get(instance->klass->methods, method_name, &method)) {
        runtime_error("Undefined method '%s'.", method_name);
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    // 调用方法
    call(ms_value_as_function(method), arg_count);
    break;
}
```

## 测试文件

### test_listcomp_filter.ms
```python
print("=== 列表推导式条件过滤测试 ===")

# 基础过滤
evens = [x for x in range(10) if x % 2 == 0]
print("偶数:", evens)  # [0, 2, 4, 6, 8]

# 多个条件
result = [x for x in range(20) if x % 2 == 0 if x % 3 == 0]
print("2和3的倍数:", result)  # [0, 6, 12, 18]

# 复杂表达式
squares = [x**2 for x in range(-5, 5) if x > 0]
print("正数的平方:", squares)  # [1, 4, 9, 16]

print("✅ 所有测试通过！")
```

### test_walrus.ms
```python
print("=== 海象运算符测试 ===")

# 基础用法
if (n := len([1, 2, 3])) > 2:
    print(f"长度是 {n}")  # 长度是 3

# 在循环中
numbers = [1, 2, 3, 4, 5]
i = 0
while (val := numbers[i] if i < len(numbers) else None) is not None:
    print(f"值: {val}")
    i += 1

print("✅ 所有测试通过！")
```

### test_class_basic.ms
```python
print("=== 类定义基础测试 ===")

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def introduce(self):
        return f"I'm {self.name}, {self.age} years old"
    
    def birthday(self):
        self.age += 1
        return f"Happy birthday! Now {self.age} years old"

# 创建实例
p = Person("Alice", 30)
print(p.name)  # Alice
print(p.age)   # 30
print(p.introduce())  # I'm Alice, 30 years old

# 修改属性
p.age = 31
print(p.age)  # 31

# 调用方法
print(p.birthday())  # Happy birthday! Now 32 years old

print("✅ 所有测试通过！")
```

### test_inheritance.ms
```python
print("=== 继承测试 ===")

class Animal:
    def __init__(self, name):
        self.name = name
    
    def speak(self):
        return "Some sound"

class Dog(Animal):
    def __init__(self, name, breed):
        super().__init__(name)
        self.breed = breed
    
    def speak(self):
        return "Woof!"
    
    def info(self):
        return f"{self.name} is a {self.breed}"

dog = Dog("Buddy", "Golden Retriever")
print(dog.name)   # Buddy
print(dog.breed)  # Golden Retriever
print(dog.speak())  # Woof!
print(dog.info())   # Buddy is a Golden Retriever

print("✅ 所有测试通过！")
```

## 实现时间表

### 今天（4-6小时）
- [ ] 列表推导式条件过滤（2小时）
- [ ] 海象运算符（2小时）

### 明天-后天（2-3天）
- [ ] 类定义基础（2-3天）

### 本周内（1-2天）
- [ ] 继承和 super()（1-2天）

### 下周（2-3天）
- [ ] 装饰器（2-3天）

### 下下周（2-3天）
- [ ] 魔术方法（2-3天）

## 总计时间
- 第一优先级完成：4-6小时
- 第二优先级完成：7-11天
- **总计**：约 2 周

---

**开始时间**: 立即  
**预计完成**: 2周后  
**当前进度**: 51% → 目标 100%
