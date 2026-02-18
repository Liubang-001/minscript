# 测试新增的运算符

print("=== 测试 // 整除运算符 ===")
print("10 // 3 =", 10 // 3)
print("20 // 4 =", 20 // 4)
print("-10 // 3 =", -10 // 3)
print()

print("=== 测试 ** 幂运算符 ===")
print("2 ** 3 =", 2 ** 3)
print("5 ** 2 =", 5 ** 2)
print("10 ** 0 =", 10 ** 0)
print("2 ** 10 =", 2 ** 10)
print()

print("=== 测试 % 取模运算符 ===")
print("10 % 3 =", 10 % 3)
print("20 % 7 =", 20 % 7)
print("15 % 5 =", 15 % 5)
print()

print("=== 组合测试 ===")
result = 2 ** 3 + 10 // 3
print("2 ** 3 + 10 // 3 =", result)

result2 = (5 + 3) ** 2
print("(5 + 3) ** 2 =", result2)

result3 = 100 // 10 % 3
print("100 // 10 % 3 =", result3)
print()

print("所有运算符测试完成！")
