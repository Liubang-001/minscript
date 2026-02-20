# 缺失功能实现计划

## 当前状态总结

### ✅ 已实现
1. ✅ 列表推导式（基础 + 三元表达式）
2. ✅ lambda 表达式（词法分析器支持）
3. ✅ for/while 的 else 子句
4. ✅ 默认参数
5. ⚠️ 海象运算符（词法分析器支持）
6. ✅ assert 语句
7. ✅ del 语句
8. ✅ 类定义（class）
9. ✅ 继承（super）
10. ⚠️ 魔术方法（部分：__init__, __eq__, __add__, __str__）

### ❌ 未实现
1. ❌ 集合推导式
2. ❌ 字典推导式
3. ❌ lambda 表达式（解析器和VM实现）
4. ❌ 海象运算符（解析器和VM实现）
5. ❌ 属性装饰器（@property）
6. ❌ 类方法（@classmethod）
7. ❌ 静态方法（@staticmethod）
8. ❌ 更多魔术方法（__repr__, __sub__, __mul__, __div__, __lt__, __gt__, __len__, __getitem__, __setitem__）

## 实现优先级

### 立即实现（今天）
1. **lambda 表达式** - 已有TOKEN，需要实现解析器和VM
2. **海象运算符 :=** - 已有TOKEN，需要实现解析器和VM
3. **集合推导式** - 类似列表推导式
4. **字典推导式** - 类似列表推导式

### 第二批（明天）
5. **@property 装饰器** - 需要装饰器系统
6. **@classmethod 装饰器** - 需要装饰器系统
7. **@staticmethod 装饰器** - 需要装饰器系统
8. **更多魔术方法** - 扩展现有系统

## 详细实现步骤

### 1. Lambda 表达式

#### 词法分析器
- ✅ TOKEN_LAMBDA 已定义

#### 解析器
```c
// 在 parser.c 中实现
static void lambda_expression(ms_parser_t* parser) {
    // lambda x, y: x + y
    // 1. 解析参数列表
    // 2. 解析冒号
    // 3. 解析表达式（不是语句块）
    // 4. 创建匿名函数
}
```

#### VM
- ✅ OP_LAMBDA 已定义
- 需要实现 OP_LAMBDA 的执行逻辑

### 2. 海象运算符 :=

#### 词法分析器
- ⚠️ TOKEN_WALRUS 需要确认

#### 解析器
```c
// 在表达式中识别 :=
// (x := 10) + 5
// 需要在 assignment 或 expression 中处理
```

#### VM
- 需要新的操作码或复用现有的赋值操作

### 3. 集合推导式

#### 语法
```python
{x for x in range(10)}
{x*2 for x in range(10) if x % 2 == 0}
```

#### 实现
- 类似列表推导式，但使用 set 而不是 list
- 需要 OP_BUILD_SET 操作码

### 4. 字典推导式

#### 语法
```python
{k: v for k, v in items}
{x: x*2 for x in range(10)}
```

#### 实现
- 需要解析 key: value 对
- 需要 OP_BUILD_DICT_COMP 操作码

### 5. 装饰器系统

#### 基础装饰器语法
```python
@decorator
def function():
    pass

# 等价于
def function():
    pass
function = decorator(function)
```

#### @property
```python
class MyClass:
    @property
    def value(self):
        return self._value
    
    @value.setter
    def value(self, val):
        self._value = val
```

#### @classmethod
```python
class MyClass:
    @classmethod
    def create(cls, arg):
        return cls(arg)
```

#### @staticmethod
```python
class MyClass:
    @staticmethod
    def utility():
        return 42
```

### 6. 更多魔术方法

需要实现的魔术方法：
- `__repr__` - 对象表示
- `__sub__` - 减法
- `__mul__` - 乘法
- `__div__` - 除法
- `__lt__` - 小于
- `__gt__` - 大于
- `__le__` - 小于等于
- `__ge__` - 大于等于
- `__len__` - 长度
- `__getitem__` - 索引访问
- `__setitem__` - 索引赋值
- `__contains__` - in 运算符
- `__iter__` - 迭代器
- `__next__` - 迭代器下一个

## 实现顺序

1. Lambda 表达式（2小时）
2. 海象运算符（1小时）
3. 集合推导式（1小时）
4. 字典推导式（1.5小时）
5. 装饰器基础框架（2小时）
6. @property（1小时）
7. @classmethod（0.5小时）
8. @staticmethod（0.5小时）
9. 更多魔术方法（3小时）

总计：约12小时工作量
