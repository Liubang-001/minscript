#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Python 3 核心语法综合示例
涵盖变量、数据类型、控制流、函数、类、异常、迭代器、装饰器等核心语法
"""

# ===================== 1. 基础数据类型与变量 =====================
# 1.1 基本类型
integer_var = 42          # 整数
float_var = 3.14159       # 浮点数
bool_var = True           # 布尔值
string_var = "Python 3"   # 字符串
none_var = None           # 空值

# 1.2 容器类型
list_var = [1, 2, 3, "Python"]          # 列表（可变有序）
tuple_var = (1, 2, 3, "Tuple")          # 元组（不可变有序）
dict_var = {"name": "Python", "version": 3.12}  # 字典（键值对）
set_var = {1, 2, 3, 3, 4}               # 集合（无序不重复）

# 字符串格式化（Python 3.6+ f-string）
formatted_str = f"版本：{dict_var['version']}，数字：{integer_var + 10}"

# ===================== 2. 控制流语句 =====================
# 2.1 条件判断
def check_number(num):
    if num > 0:
        return "正数"
    elif num < 0:
        return "负数"
    else:
        return "零"

# 2.2 循环语句
# for 循环（遍历可迭代对象）
loop_list = []
for i in range(5):  # range 生成序列
    loop_list.append(i * 2)

# while 循环
count = 0
while count < 3:
    count += 1
    if count == 2:
        continue  # 跳过本次循环
    print(f"循环计数：{count}")
    if count == 3:
        break  # 终止循环

# ===================== 3. 函数相关 =====================
# 3.1 普通函数 + 默认参数 + 关键字参数
def calculate(a: int, b: int = 10, *, c: int = 5) -> int:
    """
    简单计算函数（带类型注解）
    :param a: 必选参数
    :param b: 可选默认参数
    :param c: 仅限关键字参数（* 后的参数必须用关键字传参）
    :return: 计算结果
    """
    return a + b * c

# 3.2 可变参数
def multi_args(*args, **kwargs):
    """可变位置参数（*args）和可变关键字参数（**kwargs）"""
    total = sum(args)
    for k, v in kwargs.items():
        print(f"关键字参数 {k} = {v}")
    return total

# 3.3 匿名函数（lambda）
square = lambda x: x **2

# 3.4 装饰器（Python 3 语法糖）
def decorator_func(func):
    """简单装饰器：记录函数执行时间"""
    import time
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f"函数 {func.__name__} 执行耗时：{end - start:.6f} 秒")
        return result
    return wrapper

@decorator_func  # 装饰器语法糖
def slow_func():
    time.sleep(0.1)
    return "执行完成"

# ===================== 4. 类与面向对象 =====================
class BaseClass:
    """父类（基类）"""
    class_var = "类变量"  # 类变量（所有实例共享）
    
    def __init__(self, name):
        self.name = name  # 实例变量
        self._private_var = "受保护变量"  # 约定俗成的私有变量（单下划线）
        self.__strict_private = "严格私有变量"  # 真正私有（双下划线，名称改写）
    
    def common_method(self):
        """实例方法"""
        return f"实例方法：{self.name}"
    
    @classmethod
    def class_method(cls):
        """类方法（绑定到类，而非实例）"""
        return f"类方法：{cls.class_var}"
    
    @staticmethod
    def static_method():
        """静态方法（无类/实例绑定）"""
        return "静态方法"

class SubClass(BaseClass):
    """子类（继承）"""
    def __init__(self, name, age):
        super().__init__(name)  # 调用父类构造方法
        self.age = age
    
    def common_method(self):
        """方法重写"""
        parent_result = super().common_method()
        return f"{parent_result}，年龄：{self.age}"

# ===================== 5. 迭代器与生成器 =====================
# 5.1 迭代器（实现 __iter__ 和 __next__）
class MyIterator:
    def __init__(self, max_num):
        self.max_num = max_num
        self.current = 0
    
    def __iter__(self):
        return self
    
    def __next__(self):
        if self.current < self.max_num:
            self.current += 1
            return self.current
        raise StopIteration  # 终止迭代

# 5.2 生成器（yield 关键字，简化迭代器）
def my_generator(max_num):
    """生成器函数：按需生成数据，节省内存"""
    num = 1
    while num <= max_num:
        yield num  # 生成值，暂停函数执行
        num += 1

# ===================== 6. 异常处理 =====================
def exception_demo():
    """异常处理（try-except-else-finally）"""
    try:
        10 / 0  # 故意触发除零错误
    except ZeroDivisionError as e:
        print(f"捕获异常：{type(e).__name__} - {e}")
    except Exception as e:
        print(f"捕获其他异常：{e}")
    else:
        print("无异常时执行")
    finally:
        print("无论是否异常，最终都会执行")

# ===================== 7. 其他常用语法 =====================
# 7.1 列表推导式（简洁创建列表）
list_comp = [x*2 for x in range(5) if x % 2 == 0]

# 7.2 字典推导式
dict_comp = {k: v*2 for k, v in {"a":1, "b":2}.items()}

# 7.3 解包
a, b, *rest = [1, 2, 3, 4, 5]  # 星号解包剩余元素

# 7.4 with 语句（上下文管理器，自动释放资源）
with open("temp.txt", "w", encoding="utf-8") as f:
    f.write("Python 3 语法示例")

# ===================== 8. 代码执行入口 =====================
if __name__ == "__main__":
    # 打印基础类型
    print("===== 基础类型 =====")
    print(f"字符串格式化：{formatted_str}")
    print(f"集合去重：{set_var}")
    
    # 控制流
    print("\n===== 控制流 =====")
    print(f"数字判断：{check_number(-5)}")
    print(f"循环结果：{loop_list}")
    
    # 函数
    print("\n===== 函数 =====")
    print(f"普通函数：{calculate(5, c=8)}")
    print(f"可变参数：{multi_args(1,2,3, name='Python', version=3)}")
    print(f"lambda 平方：{square(6)}")
    print(f"装饰器函数：{slow_func()}")
    
    # 类
    print("\n===== 类 =====")
    sub_obj = SubClass("Python", 31)
    print(f"子类方法：{sub_obj.common_method()}")
    print(f"类方法：{BaseClass.class_method()}")
    print(f"静态方法：{BaseClass.static_method()}")
    
    # 迭代器/生成器
    print("\n===== 迭代器/生成器 =====")
    iterator = MyIterator(3)
    print(f"迭代器：{list(iterator)}")
    generator = my_generator(3)
    print(f"生成器：{list(generator)}")
    
    # 异常处理
    print("\n===== 异常处理 =====")
    exception_demo()
    
    # 推导式与解包
    print("\n===== 推导式与解包 =====")
    print(f"列表推导式：{list_comp}")
    print(f"字典推导式：{dict_comp}")
    print(f"解包：a={a}, b={b}, rest={rest}")