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
