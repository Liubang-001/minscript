print("=== 简单 Match/Case 测试 ===")
print()

# 测试 1: 基本 match/case
print("测试 1: 基本 match/case")
day = 3
match day:
    case 1:
        print("  星期一")
    case 2:
        print("  星期二")
    case 3:
        print("  星期三")
    case _:
        print("  其他")
print()

# 测试 2: 字符串匹配
print("测试 2: 字符串匹配")
status = "active"
match status:
    case "active":
        print("  活跃")
    case "inactive":
        print("  不活跃")
    case _:
        print("  未知")
print()

print("=== 测试完成 ===")
