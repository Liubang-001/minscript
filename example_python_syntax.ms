"""
Python 列表推导式 + 三元表达式示例
演示: result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
"""

print("==================================================")
print("Python 风格的列表推导式 + 三元表达式")
print("==================================================")
print()

# 示例1: 判断奇偶数（与 Python 完全相同的语法）
print("示例1: 判断奇偶数")
print("--------------------------------------------------")
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]

print("代码:")
print("numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]")
print("result_list = [\"偶数\" if n % 2 == 0 else \"奇数\" for n in numbers]")
print()
print("结果:")
for i in range(len(numbers)):
    print(f"  {numbers[i]} -> {result_list[i]}")
print()

# 示例2: 成绩评级
print("示例2: 成绩评级")
print("--------------------------------------------------")
scores = [95, 87, 76, 54, 68, 42, 91]
grades = ["优秀" if s >= 90 else ("良好" if s >= 80 else ("及格" if s >= 60 else "不及格")) for s in scores]

print("代码:")
print("scores = [95, 87, 76, 54, 68, 42, 91]")
print("grades = [\"优秀\" if s >= 90 else (\"良好\" if s >= 80 else (\"及格\" if s >= 60 else \"不及格\")) for s in scores]")
print()
print("结果:")
for i in range(len(scores)):
    print(f"  分数 {scores[i]} -> {grades[i]}")
print()

# 示例3: 数值转换
print("示例3: 数值转换")
print("--------------------------------------------------")
values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
transformed = [n * 2 if n % 2 == 0 else n * 3 for n in values]

print("代码:")
print("values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]")
print("transformed = [n * 2 if n % 2 == 0 else n * 3 for n in values]")
print("规则: 偶数乘以2，奇数乘以3")
print()
print("结果:")
for i in range(len(values)):
    op = "x2" if values[i] % 2 == 0 else "x3"
    print(f"  {values[i]} {op} = {transformed[i]}")
print()

# 示例4: 温度分类
print("示例4: 温度分类")
print("--------------------------------------------------")
temperatures = [5, 15, 25, 35, 45]
categories = ["冷" if t < 10 else ("凉" if t < 20 else ("温" if t < 30 else "热")) for t in temperatures]

print("代码:")
print("temperatures = [5, 15, 25, 35, 45]")
print("categories = [\"冷\" if t < 10 else (\"凉\" if t < 20 else (\"温\" if t < 30 else \"热\")) for t in temperatures]")
print()
print("结果:")
for i in range(len(temperatures)):
    print(f"  {temperatures[i]}°C -> {categories[i]}")
print()

print("==================================================")
print("所有示例完成！")
print("==================================================")
