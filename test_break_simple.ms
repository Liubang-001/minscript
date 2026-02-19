# 简单测试

print("测试 1: while break")
i = 0
while i < 10:
    if i == 3:
        break
    print(i)
    i = i + 1
print("结束")

print()
print("测试 2: for break")
for i in range(10):
    if i == 3:
        break
    print(i)
print("结束")

print()
print("测试 3: while continue")
i = 0
while i < 5:
    i = i + 1
    if i == 3:
        continue
    print(i)
print("结束")

print()
print("测试 4: for continue")
for i in range(5):
    if i == 2:
        continue
    print(i)
print("结束")
