# 完整的多行字符串测试

print("=== 多行字符串测试 ===")
print()

# 1. 基本多行字符串
print("1. 基本多行字符串")
text = """第一行
第二行
第三行"""
print(text)
print()

# 2. 带缩进的多行字符串
print("2. 带缩进的多行字符串")
code = """def hello():
    print("Hello")
    print("World")"""
print(code)
print()

# 3. 空的多行字符串
print("3. 空的多行字符串")
empty = """"""
print("空字符串长度:", len(empty))
print()

# 4. 函数文档字符串
print("4. 函数文档字符串")
def calculate(a, b):
    """计算两个数的和
    
    参数:
        a: 第一个数
        b: 第二个数
    
    返回:
        两数之和
    """
    return a + b

result = calculate(10, 20)
print("计算结果:", result)
print()

# 5. 多行字符串作为注释
"""
这是一个多行注释
在 Python 中，如果多行字符串不赋值给变量
它就相当于注释
"""

# 6. 混合使用单行和多行字符串
print("5. 混合使用")
s1 = "单行"
s2 = """多行
字符串"""
print("s1:", s1)
print("s2:", s2)
print()

print("=== 所有测试完成 ===")
