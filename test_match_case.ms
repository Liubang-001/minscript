"""
Match/Case 语句测试
Python 3.10+ 风格的模式匹配
"""

print("=== Match/Case 语句测试 ===")
print()

# 基本 match/case
print("1. 基本 match/case")
day = 3
match day:
    case 1:
        print("  星期一")
    case 2:
        print("  星期二")
    case 3:
        print("  星期三")
    case 4:
        print("  星期四")
    case 5:
        print("  星期五")
    case _:
        print("  周末")
print()

# 使用通配符
print("2. 使用通配符")
value = 100
match value:
    case 1:
        print("  值是 1")
    case 2:
        print("  值是 2")
    case _:
        print(f"  值是其他: {value}")
print()

# 多个 case
print("3. 多个 case")
status = "active"
match status:
    case "active":
        print("  状态: 活跃")
    case "inactive":
        print("  状态: 不活跃")
    case "pending":
        print("  状态: 待处理")
    case _:
        print("  状态: 未知")
print()

# 数字范围
print("4. 数字范围")
score = 85
match score:
    case 90:
        print("  优秀")
    case 80:
        print("  良好")
    case 70:
        print("  及格")
    case _:
        print(f"  其他分数: {score}")
print()

print("=== Match/Case 语句测试完成 ===")
