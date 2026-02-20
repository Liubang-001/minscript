"""
三元表达式测试
语法: value_if_true if condition else value_if_false
"""

print("=== 三元表达式测试 ===")
print()

# 基本三元表达式
x = 10
result = "大于5" if x > 5 else "小于等于5"
print(f"x = {x}, 结果: {result}")
print()

# 嵌套三元表达式
age = 25
status = "成人" if age >= 18 else "未成年"
print(f"年龄 {age}: {status}")
print()

# 在列表中使用
numbers = [1, 2, 3, 4, 5]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]
print(f"数字分类: {result_list}")
print()

# 在函数中使用
def get_grade(score):
    return "及格" if score >= 60 else "不及格"

print(f"成绩 85: {get_grade(85)}")
print(f"成绩 45: {get_grade(45)}")
print()

# 复杂条件
value = 42
category = "大" if value > 50 else ("中" if value > 25 else "小")
print(f"值 {value} 的分类: {category}")
print()

print("=== 三元表达式测试完成 ===")
