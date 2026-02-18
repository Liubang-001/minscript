#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Python 3 语法特性综合示例
涵盖：基础语法、数据结构、控制流、函数、类、异常、文件IO、装饰器、
生成器、迭代器、异步编程、类型注解等。
"""

import asyncio
import json
from typing import Optional, Union, List, Dict, Any, Generator, AsyncGenerator

# ==================== 1. 基础语法 ====================
def basic_syntax():
    """演示变量、运算符、字面量、注释等基础语法"""
    # 这是单行注释
    """
    这是多行字符串，也可以作为文档字符串或块注释
    """

    # 变量赋值
    a = 10          # 整数
    b = 3.14        # 浮点数
    c = "Hello"     # 字符串
    d = True        # 布尔值
    e = None        # 空值

    # 运算符
    sum_ab = a + b
    power = a ** 2          # 乘方
    floor_div = a // 3      # 整除
    remainder = a % 3       # 取余
    is_equal = (a == 10)    # 比较

    # 海象运算符 (Python 3.8+)
    if (n := len(c)) > 4:
        print(f"字符串长度 {n} > 4")

    # f-string 格式化 (Python 3.6+)
    print(f"a = {a}, b = {b:.2f}, c = {c!r}")

    # 类型注解 (仅注解，运行时无强制)
    x: int = 5
    y: str = "world"

# ==================== 2. 数据结构 ====================
def data_structures():
    """演示列表、元组、字典、集合、推导式"""
    # 列表
    fruits = ["apple", "banana", "cherry"]
    fruits.append("date")
    fruits[0] = "apricot"

    # 元组 (不可变)
    point = (10, 20)
    x, y = point          # 解包

    # 字典
    person = {
        "name": "Alice",
        "age": 30,
        "city": "New York"
    }
    person["email"] = "alice@example.com"

    # 集合
    numbers = {1, 2, 3, 3, 4}   # 自动去重 -> {1,2,3,4}
    numbers.add(5)

    # 推导式
    squares = [x**2 for x in range(5)]          # 列表推导式
    even_squares = {x**2 for x in range(5) if x % 2 == 0}  # 集合推导式
    square_dict = {x: x**2 for x in range(5)}   # 字典推导式

    # 切片
    sublist = fruits[1:3]        # ['banana', 'cherry']
    reverse = fruits[::-1]        # 反转

    # 多变量赋值与交换
    a, b = 1, 2
    a, b = b, a                   # 交换

# ==================== 3. 控制流 ====================
def control_flow():
    """演示 if-elif-else, for, while, break, continue, else 子句"""
    # if-elif-else
    score = 85
    if score >= 90:
        grade = 'A'
    elif score >= 80:
        grade = 'B'
    else:
        grade = 'C'

    # for 循环
    for i in range(3):
        print(f"for 循环第 {i} 次")

    # while 循环
    count = 0
    while count < 3:
        print(f"while 循环第 {count} 次")
        count += 1

    # break / continue
    for i in range(5):
        if i == 2:
            continue          # 跳过本次循环
        if i == 4:
            break             # 终止循环
        print(f"i = {i}")

    # for/while 的 else 子句：循环正常结束（未 break）时执行
    for i in range(3):
        print(i)
    else:
        print("循环正常结束")

    # 使用 enumerate 获取索引
    for idx, fruit in enumerate(["apple", "banana"]):
        print(f"{idx}: {fruit}")

# ==================== 4. 函数 ====================
def function_demo():
    """函数定义：多种参数类型、lambda、文档字符串、类型注解"""

    # 普通函数
    def add(a: int, b: int) -> int:
        """返回两个数的和"""
        return a + b

    # 默认参数
    def greet(name: str, greeting: str = "Hello") -> str:
        return f"{greeting}, {name}"

    # 可变位置参数 (*args) 和可变关键字参数 (**kwargs)
    def sum_all(*args: int) -> int:
        return sum(args)

    def print_info(**kwargs: Any) -> None:
        for key, value in kwargs.items():
            print(f"{key}: {value}")

    # 关键字参数（强制使用关键字）
    def safe_divide(a: int, b: int, *, precision: int = 2) -> float:
        """* 后面的参数必须使用关键字传递"""
        return round(a / b, precision)

    # lambda 函数
    multiply = lambda x, y: x * y

    # 调用示例
    print(add(3, 5))
    print(greet("Bob"))
    print(greet("Bob", greeting="Hi"))
    print(sum_all(1, 2, 3, 4))
    print_info(name="Alice", age=30)
    print(safe_divide(10, 3, precision=3))
    print(multiply(4, 5))

    # 文档字符串
    print(add.__doc__)

# ==================== 5. 类 ====================
class Person:
    """人的基类"""
    # 类属性
    species = "Homo sapiens"

    def __init__(self, name: str, age: int):
        """构造方法"""
        self.name = name          # 实例属性
        self._age = age            # 受保护的属性（约定）
        self.__private = "secret"  # 私有属性（名称改写）

    def introduce(self) -> str:
        """实例方法"""
        return f"我叫 {self.name}，今年 {self._age} 岁。"

    @property
    def age(self) -> int:
        """属性装饰器：将方法变成属性访问"""
        return self._age

    @age.setter
    def age(self, value: int) -> None:
        """属性的 setter"""
        if value < 0:
            raise ValueError("年龄不能为负")
        self._age = value

    @classmethod
    def from_birth_year(cls, name: str, birth_year: int):
        """类方法：工厂方法，通过出生年份创建实例"""
        import datetime
        current_year = datetime.datetime.now().year
        return cls(name, current_year - birth_year)

    @staticmethod
    def is_adult(age: int) -> bool:
        """静态方法：与类相关但不访问实例或类属性"""
        return age >= 18

    def __str__(self) -> str:
        """用户友好的字符串表示"""
        return f"Person(name={self.name}, age={self.age})"

    def __repr__(self) -> str:
        """开发者友好的字符串表示"""
        return f"Person('{self.name}', {self.age})"

    def __add__(self, other: 'Person') -> str:
        """运算符重载：两个 Person 相加返回组合名"""
        return f"{self.name} & {other.name}"


# 继承
class Student(Person):
    """学生类，继承自 Person"""
    def __init__(self, name: str, age: int, student_id: str):
        super().__init__(name, age)      # 调用父类构造
        self.student_id = student_id

    def introduce(self) -> str:
        """重写父类方法"""
        return f"我是学生 {self.name}，学号 {self.student_id}。"


def class_demo():
    """演示类的使用"""
    # 创建实例
    p = Person("Alice", 25)
    print(p.introduce())
    print(p.age)                # 使用 property
    p.age = 26                  # 使用 setter
    print(p)

    # 类方法
    p2 = Person.from_birth_year("Bob", 1990)
    print(p2)

    # 静态方法
    print(Person.is_adult(20))

    # 运算符重载
    print(p + p2)

    # 继承
    s = Student("Charlie", 20, "S12345")
    print(s.introduce())

    # 私有属性（名称改写）
    # print(p.__private)        # 会报错
    print(p._Person__private)    # 可以访问，但不推荐

# ==================== 6. 异常处理 ====================
# 自定义异常
class MyCustomError(Exception):
    """自定义异常类"""
    def __init__(self, message: str, code: int):
        self.message = message
        self.code = code
        super().__init__(f"{message} (code: {code})")

def exception_demo():
    """演示 try-except-else-finally 和 raise"""
    try:
        x = int(input("请输入一个整数: "))
        result = 10 / x
    except ValueError:
        print("输入的不是整数")
    except ZeroDivisionError:
        print("不能除以零")
    else:
        # 没有异常时执行
        print(f"结果是 {result}")
    finally:
        # 无论是否异常都会执行
        print("清理工作...")

    # 手动抛出异常
    try:
        raise MyCustomError("发生了自定义错误", 42)
    except MyCustomError as e:
        print(f"捕获到自定义异常: {e}")

# ==================== 7. 文件 I/O ====================
def file_io_demo():
    """文件读写操作"""
    # 写文件
    with open("demo.txt", "w", encoding="utf-8") as f:
        f.write("Hello, World!\n")
        f.write("第二行\n")

    # 读文件
    with open("demo.txt", "r", encoding="utf-8") as f:
        content = f.read()
        print(content)

    # 逐行读取
    with open("demo.txt", "r", encoding="utf-8") as f:
        for line in f:
            print(line.strip())

    # JSON 处理
    data = {"name": "Alice", "age": 30}
    with open("demo.json", "w") as f:
        json.dump(data, f)

    with open("demo.json", "r") as f:
        loaded = json.load(f)
        print(loaded)

# ==================== 8. 上下文管理器 ====================
class ManagedResource:
    """自定义上下文管理器"""
    def __enter__(self):
        print("进入上下文，获取资源")
        # 可以返回资源对象
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("退出上下文，释放资源")
        if exc_type:
            print(f"处理异常: {exc_val}")
        # 返回 True 表示异常已被处理，不再传播
        return False   # 通常返回 False 让异常继续传播

def context_manager_demo():
    """使用自定义上下文管理器"""
    with ManagedResource() as res:
        print("在上下文中执行操作")
        # 模拟异常
        # raise ValueError("出错了")

# ==================== 9. 装饰器 ====================
def simple_decorator(func):
    """无参数装饰器"""
    def wrapper(*args, **kwargs):
        print("调用前")
        result = func(*args, **kwargs)
        print("调用后")
        return result
    return wrapper

def decorator_with_args(arg1, arg2):
    """带参数的装饰器"""
    def decorator(func):
        def wrapper(*args, **kwargs):
            print(f"装饰器参数: {arg1}, {arg2}")
            return func(*args, **kwargs)
        return wrapper
    return decorator

@simple_decorator
def say_hello(name):
    print(f"Hello, {name}")

@decorator_with_args("hello", "world")
def greet(name):
    print(f"Greetings, {name}")

# 类装饰器
def add_repr(cls):
    """类装饰器：为类添加 __repr__ 方法"""
    def __repr__(self):
        attrs = ", ".join(f"{k}={v}" for k, v in self.__dict__.items())
        return f"{cls.__name__}({attrs})"
    cls.__repr__ = __repr__
    return cls

@add_repr
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

def decorator_demo():
    say_hello("Alice")
    greet("Bob")
    p = Point(3, 4)
    print(p)   # 使用了装饰器添加的 __repr__

# ==================== 10. 生成器 ====================
def count_up_to(n: int) -> Generator[int, None, None]:
    """生成器函数：从1数到n"""
    i = 1
    while i <= n:
        yield i
        i += 1

def generator_demo():
    """演示生成器"""
    for num in count_up_to(5):
        print(num)

    # 生成器表达式
    squares = (x**2 for x in range(5))
    print(list(squares))   # [0, 1, 4, 9, 16]

    # 手动迭代
    gen = count_up_to(3)
    print(next(gen))   # 1
    print(next(gen))   # 2
    print(next(gen))   # 3

# ==================== 11. 迭代器 ====================
class CountDown:
    """自定义迭代器：倒数"""
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

def iterator_demo():
    cd = CountDown(5)
    for num in cd:
        print(num)

# ==================== 12. 异步编程 ====================
async def fetch_data(delay: int, name: str) -> str:
    """模拟异步IO操作"""
    print(f"开始获取 {name}...")
    await asyncio.sleep(delay)
    print(f"完成获取 {name}")
    return f"{name} 数据"

async def async_demo():
    """异步函数"""
    # 并发执行多个任务
    task1 = asyncio.create_task(fetch_data(2, "任务1"))
    task2 = asyncio.create_task(fetch_data(1, "任务2"))

    # 等待所有任务完成
    results = await asyncio.gather(task1, task2)
    print(results)

# 异步上下文管理器
class AsyncResource:
    async def __aenter__(self):
        print("异步进入")
        await asyncio.sleep(0.1)
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        print("异步退出")
        await asyncio.sleep(0.1)

async def async_context_demo():
    async with AsyncResource() as res:
        print("使用异步资源")

# 异步生成器 (Python 3.6+)
async def async_counter(n: int) -> AsyncGenerator[int, None]:
    for i in range(n):
        await asyncio.sleep(0.1)
        yield i

async def async_gen_demo():
    async for num in async_counter(5):
        print(num)

# ==================== 13. 类型提示进阶 ====================
def type_hints_demo():
    """展示类型注解的更多用法"""
    # 变量注解
    name: str = "Alice"
    ages: List[int] = [25, 30, 35]
    info: Dict[str, Union[str, int]] = {"name": "Bob", "age": 28}
    maybe: Optional[str] = None   # 可以是 str 或 None

    # 函数注解已在前面展示
x = 1
# ==================== 14. 其他语法 ====================
def miscellaneous():
    """展示一些杂项语法"""
    # global / nonlocal
    x = 10  # 全局变量

    def outer():
        x = 20  # 嵌套作用域变量
        def inner():
            nonlocal x   # 引用 outer 的 x
            x += 5
            print("inner x =", x)
        inner()
        print("outer x =", x)

    def change_global():
        global x
        x += 100
        print("global x =", x)

    outer()
    change_global()

    # 断言
    assert 1 + 1 == 2, "数学错误"

    # pass 空语句
    def empty():
        pass

    # del 删除变量或元素
    lst = [1, 2, 3]
    del lst[1]
    print(lst)

    # eval / exec (谨慎使用)
    code = "print('eval 执行')"
    eval(code)   # 执行表达式
    exec("a = 5; print(a)")   # 执行语句

# ==================== 主程序入口 ====================
if __name__ == "__main__":
    print("====== 基础语法 ======")
    basic_syntax()

    print("\n====== 数据结构 ======")
    data_structures()

    print("\n====== 控制流 ======")
    control_flow()

    print("\n====== 函数 ======")
    function_demo()

    print("\n====== 类 ======")
    class_demo()

    print("\n====== 异常处理 ======")
    exception_demo()

    print("\n====== 文件 I/O ======")
    file_io_demo()

    print("\n====== 上下文管理器 ======")
    context_manager_demo()

    print("\n====== 装饰器 ======")
    decorator_demo()

    print("\n====== 生成器 ======")
    generator_demo()

    print("\n====== 迭代器 ======")
    iterator_demo()

    print("\n====== 异步编程 ======")
    # 运行异步函数
    asyncio.run(async_demo())
    asyncio.run(async_context_demo())
    asyncio.run(async_gen_demo())

    print("\n====== 其他语法 ======")
    miscellaneous()

    print("\n所有演示完成。")