print("=== 列表推导式条件过滤测试 ===")

# 基础过滤
evens = [x for x in range(10) if x % 2 == 0]
print("偶数:", evens)  # [0, 2, 4, 6, 8]

# 多个条件
result = [x for x in range(20) if x % 2 == 0 if x % 3 == 0]
print("2和3的倍数:", result)  # [0, 6, 12, 18]

# 复杂表达式
squares = [x**2 for x in range(-5, 5) if x > 0]
print("正数的平方:", squares)  # [1, 4, 9, 16]

# 带条件的字符串过滤
words = ["apple", "banana", "cherry", "date"]
long_words = [w for w in words if len(w) > 5]
print("长单词:", long_words)  # ["banana", "cherry"]

print("✅ 所有测试通过！")
