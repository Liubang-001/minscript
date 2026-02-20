# 面向对象编程（OOP）实现计划

## 概述
本文档详细说明如何实现 Python 的面向对象编程特性，包括类定义、继承、装饰器和魔术方法。

## 实现优先级

### 第一阶段：基础类系统（2-3天）
1. 类定义（class）
2. 实例化（__init__）
3. 实例属性和方法
4. self 参数

### 第二阶段：继承系统（1-2天）
5. 单继承
6. super() 函数
7. 方法重写

### 第三阶段：装饰器（1-2天）
8. @property
9. @classmethod
10. @staticmethod

### 第四阶段：魔术方法（2-3天）
11. __str__ 和 __repr__
12. 运算符重载（__add__, __sub__, __mul__ 等）
13. 比较运算符（__eq__, __lt__, __gt__ 等）
14. 容器方法（__len__, __getitem__, __setitem__）

## 数据结构设计

### 1. 类对象（ms_class_t）
```c
typedef struct ms_class {
    char* name;                    // 类名
    struct ms_class* parent;       // 父类
    ms_dict_t* methods;            // 方法字典
    ms_dict_t* class_attrs;        // 类属性
    ms_function_t* init;           // __init__ 方法
} ms_class_t;
```

### 2. 实例对象（ms_instance_t）
```c
typedef struct ms_instance {
    ms_class_t* klass;             // 所属类
    ms_dict_t* attrs;              // 实例属性
} ms_instance_t;
```

### 3. 绑定方法（ms_bound_method_t）
```c
typedef struct ms_bound_method {
    ms_instance_t* receiver;       // self
    ms_function_t* method;         // 方法函数
} ms_bound_method_t;
```

### 4. 属性对象（ms_property_t）
```c
typedef struct ms_property {
    ms_function_t* getter;         // getter 方法
    ms_function_t* setter;         // setter 方法（可选）
    ms_function_t* deleter;        // deleter 方法（可选）
} ms_property_t;
```

## 实现步骤

### 阶段 1：基础类系统

#### 1.1 扩展值类型
在 `miniscript.h` 中添加新的值类型：
```c
typedef enum {
    // ... 现有类型 ...
    MS_VAL_CLASS,
    MS_VAL_INSTANCE,
    MS_VAL_BOUND_METHOD,
    MS_VAL_PROPERTY,
} ms_value_type_t;
```

#### 1.2 添加操作码
在 `vm.h` 中添加：
```c
OP_CLASS,           // 创建类
OP_GET_PROPERTY,    // 获取属性
OP_SET_PROPERTY,    // 设置属性
OP_METHOD,          // 定义方法
OP_INVOKE,          // 调用方法
OP_INHERIT,         // 继承
OP_GET_SUPER,       // 获取父类方法
```

#### 1.3 解析器实现
在 `parser.c` 中添加 `class_declaration()` 函数：
```c
static void class_declaration() {
    // 1. 解析类名
    consume(TOKEN_IDENTIFIER, "Expect class name.");
    uint8_t name_constant = identifier_constant(&parser.previous);
    
    // 2. 检查是否有父类
    if (match(TOKEN_LEFT_PAREN)) {
        consume(TOKEN_IDENTIFIER, "Expect superclass name.");
        // 处理继承
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after superclass.");
    }
    
    // 3. 创建类对象
    emit_bytes(OP_CLASS, name_constant);
    
    // 4. 解析类体
    consume(TOKEN_COLON, "Expect ':' after class name.");
    consume(TOKEN_NEWLINE, "Expect newline after ':'.");
    consume(TOKEN_INDENT, "Expect indent after class declaration.");
    
    // 5. 解析方法
    while (!check(TOKEN_DEDENT) && !check(TOKEN_EOF)) {
        if (match(TOKEN_DEF)) {
            method();
        }
    }
    
    consume(TOKEN_DEDENT, "Expect dedent after class body.");
}
```

#### 1.4 虚拟机实现
在 `vm.c` 中实现类相关操作：
```c
case OP_CLASS: {
    ms_class_t* klass = ms_class_new(READ_STRING());
    push(ms_value_class(klass));
    break;
}

case OP_GET_PROPERTY: {
    if (!ms_value_is_instance(peek(0))) {
        runtime_error("Only instances have properties.");
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    ms_instance_t* instance = ms_value_as_instance(peek(0));
    const char* name = READ_STRING();
    
    ms_value_t value;
    if (ms_dict_get(instance->attrs, name, &value)) {
        pop(); // instance
        push(value);
        break;
    }
    
    // 查找方法
    if (bind_method(instance->klass, name)) {
        break;
    }
    
    runtime_error("Undefined property '%s'.", name);
    return MS_RESULT_RUNTIME_ERROR;
}
```

### 阶段 2：继承系统

#### 2.1 解析继承语法
```python
class Student(Person):
    def __init__(self, name, age, student_id):
        super().__init__(name, age)
        self.student_id = student_id
```

#### 2.2 实现 super()
```c
static bool invoke_from_class(ms_class_t* klass, const char* name, int arg_count) {
    ms_value_t method;
    if (!ms_dict_get(klass->methods, name, &method)) {
        runtime_error("Undefined property '%s'.", name);
        return false;
    }
    return call(ms_value_as_function(method), arg_count);
}

case OP_GET_SUPER: {
    const char* name = READ_STRING();
    ms_class_t* superclass = ms_value_as_class(pop());
    
    if (!bind_method(superclass, name)) {
        return MS_RESULT_RUNTIME_ERROR;
    }
    break;
}
```

### 阶段 3：装饰器

#### 3.1 @property 实现
```python
class Person:
    @property
    def age(self):
        return self._age
    
    @age.setter
    def age(self, value):
        if value < 0:
            raise ValueError("Age cannot be negative")
        self._age = value
```

解析器处理：
```c
static void decorator() {
    consume(TOKEN_AT, "Expect '@'.");
    consume(TOKEN_IDENTIFIER, "Expect decorator name.");
    
    if (identifiers_equal(&parser.previous, "property")) {
        // 解析 getter
        consume(TOKEN_NEWLINE, "Expect newline.");
        consume(TOKEN_DEF, "Expect method definition.");
        method();
        
        // 创建 property 对象
        emit_byte(OP_PROPERTY);
    }
}
```

#### 3.2 @classmethod 和 @staticmethod
```python
class Math:
    @classmethod
    def from_string(cls, s):
        return cls(int(s))
    
    @staticmethod
    def add(a, b):
        return a + b
```

### 阶段 4：魔术方法

#### 4.1 __str__ 和 __repr__
```c
case OP_PRINT: {
    ms_value_t value = pop();
    
    if (ms_value_is_instance(value)) {
        ms_instance_t* instance = ms_value_as_instance(value);
        ms_value_t str_method;
        
        if (ms_dict_get(instance->klass->methods, "__str__", &str_method)) {
            // 调用 __str__ 方法
            push(value);
            call(ms_value_as_function(str_method), 0);
            value = pop();
        }
    }
    
    print_value(value);
    printf("\n");
    break;
}
```

#### 4.2 运算符重载
```c
case OP_ADD: {
    if (ms_value_is_instance(peek(1))) {
        ms_instance_t* instance = ms_value_as_instance(peek(1));
        ms_value_t add_method;
        
        if (ms_dict_get(instance->klass->methods, "__add__", &add_method)) {
            ms_value_t b = pop();
            ms_value_t a = pop();
            push(a);
            push(b);
            call(ms_value_as_function(add_method), 1);
            break;
        }
    }
    
    // 默认加法实现
    binary_op(OP_ADD);
    break;
}
```

## 测试用例

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

### test_inheritance.ms
```python
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

dog = Dog("Buddy", "Golden Retriever")
print(dog.name)  # Buddy
print(dog.speak())  # Woof!
```

### test_property.ms
```python
class Circle:
    def __init__(self, radius):
        self._radius = radius
    
    @property
    def radius(self):
        return self._radius
    
    @radius.setter
    def radius(self, value):
        if value < 0:
            raise ValueError("Radius cannot be negative")
        self._radius = value
    
    @property
    def area(self):
        return 3.14159 * self._radius ** 2

c = Circle(5)
print(c.radius)  # 5
print(c.area)  # 78.53975
c.radius = 10
print(c.area)  # 314.159
```

### test_magic_methods.ms
```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __str__(self):
        return f"Point({self.x}, {self.y})"
    
    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

p1 = Point(1, 2)
p2 = Point(3, 4)
p3 = p1 + p2
print(p3)  # Point(4, 6)
print(p1 == Point(1, 2))  # True
```

## 实现时间估算

- 阶段 1（基础类系统）：2-3天
- 阶段 2（继承系统）：1-2天
- 阶段 3（装饰器）：1-2天
- 阶段 4（魔术方法）：2-3天

总计：6-10天

## 下一步行动

1. 扩展值类型系统，添加 CLASS、INSTANCE、BOUND_METHOD 类型
2. 实现类对象和实例对象的创建和管理
3. 实现属性访问和方法调用
4. 实现继承和 super()
5. 实现装饰器
6. 实现魔术方法

每完成一个阶段，立即编写测试用例并验证功能。
