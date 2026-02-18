# 测试嵌套 for 循环

print("测试简单 for 循环:")
for i in range(3):
    print("i =", i)

print()
print("测试嵌套 for 循环:")
for i in range(2):
    print("外层 i =", i)
    for j in range(2):
        print("  内层 j =", j)
