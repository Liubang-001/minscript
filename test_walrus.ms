print("=== 海象运算符测试 ===")

# 基础用法
if (n := len([1, 2, 3])) > 2:
    print(f"长度是 {n}")  # 长度是 3

# 在表达式中
x = (y := 10) + 5
print(f"x = {x}, y = {y}")  # x = 15, y = 10

# 在列表推导式中
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
results = [(n, n**2) for n in numbers if (square := n**2) > 25]
print("平方大于25的数:", results)

print("✅ 所有测试通过！")
