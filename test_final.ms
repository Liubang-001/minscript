"""
MiniScript 功能测试
测试本次更新的所有新功能
"""

print("=== MiniScript 功能测试 ===")
print()

# 1. 测试新运算符
print("1. 新运算符测试")
print("  10 // 3 =", 10 // 3)
print("  2 ** 10 =", 2 ** 10)
print("  10 % 3 =", 10 % 3)
print()

# 2. 测试多行字符串
print("2. 多行字符串测试")
text = """这是一个
多行字符串
测试"""
print("  多行字符串:", text)
print()

# 3. 测试空行
print("3. 空行测试")

def test_empty():

    a = 1
    
    b = 2
    
    return a + b

result = test_empty()
print("  函数结果:", result)
print()

# 4. 测试文档字符串
print("4. 文档字符串测试")

def documented_function():
    """这是一个有文档的函数
    
    功能: 打印消息
    """
    print("  函数执行成功")

documented_function()
print()

# 5. 综合测试
print("5. 综合测试")

def calculate(n):
    """计算 n 的平方和立方"""
    
    square = n ** 2
    cube = n ** 3
    
    print(f"  {n} 的平方: {square}")
    print(f"  {n} 的立方: {cube}")

calculate(5)
print()

print("=== 所有测试完成 ===")
