# 测试函数体内的空行

def test_function():
    a = 1
    
    b = 2
    
    c = 3
    print(a, b, c)

test_function()

# 测试 if 语句中的空行
def test_if():
    x = 5
    
    if x > 0:
        print("positive")
        
        print("still positive")
    
    print("done")

test_if()

# 测试 while 循环中的空行
def test_while():
    i = 0
    
    while i < 3:
        print(i)
        
        i = i + 1
    
    print("while done")

test_while()

# 测试 for 循环中的空行
def test_for():
    nums = [1, 2, 3]
    
    for n in nums:
        print(n)
        
        print(n * 2)
    
    print("for done")

test_for()

print("All tests passed!")




# ==================== 1. 基础语法 ====================
def basic_syntax():

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

basic_syntax()