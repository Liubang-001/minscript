# 测试 lambda 表达式

# 简单lambda
add = lambda x, y: x + y
print("add(3, 5) =", add(3, 5))

# 单参数lambda
square = lambda x: x * x
print("square(4) =", square(4))

# 无参数lambda
get_42 = lambda: 42
print("get_42() =", get_42())

# lambda作为参数
numbers = [1, 2, 3, 4, 5]
# squared = list(map(lambda x: x * x, numbers))
# print("squared =", squared)

# lambda在表达式中
result = (lambda x: x + 10)(5)
print("(lambda x: x + 10)(5) =", result)

# 多个lambda
funcs = [lambda x: x + 1, lambda x: x * 2, lambda x: x ** 2]
print("funcs[0](5) =", funcs[0](5))
print("funcs[1](5) =", funcs[1](5))
print("funcs[2](5) =", funcs[2](5))
