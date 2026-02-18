# 测试动态导入功能

print("=== 动态导入测试 ===")
print()

# 测试1: 基本导入
print("测试1: 基本导入")
import example
print("✓ 导入成功")
print()

# 测试2: 调用 greet 函数
print("测试2: 调用 greet 函数")
msg1 = example.greet("MiniScript")
print(msg1)
print()

# 测试3: 调用 add 函数
print("测试3: 调用 add 函数")
sum_val = example.add(15, 25)
print(sum_val)
print()

# 测试4: 调用 multiply 函数
print("测试4: 调用 multiply 函数")
prod_val = example.multiply(7, 8)
print(prod_val)
print()

# 测试5: 使用别名
print("测试5: 使用别名")
import example as ex
msg2 = ex.greet("World")
print(msg2)
print()

# 测试6: 多个操作
print("测试6: 多个操作")
a = ex.add(100, 200)
b = ex.multiply(a, 2)
print(b)
print()

print("=== 所有测试通过 ===")
