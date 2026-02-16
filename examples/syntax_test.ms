# MiniScript 语法测试文件
# 测试所有当前支持的语法特性

# ===== 基本数据类型 =====
print "=== 基本数据类型 ==="

# 布尔值
print true
print false

# 空值
print nil

# 整数
print 42
print -17
print 0

# 浮点数
print 3.14
print -2.5
print 0.0

# 字符串
print "Hello"
print "World"
print ""
print "包含中文的字符串"

# ===== 算术运算 =====
print "=== 算术运算 ==="

# 加法
print 1 + 2
print 3.5 + 1.5
print -5 + 10

# 减法
print 10 - 3
print 5.5 - 2.2
print 0 - 7

# 乘法
print 4 * 5
print 2.5 * 4
print -3 * 2

# 除法
print 15 / 3
print 7.5 / 2.5
print -8 / 2

# 复合运算
print 1 + 2 * 3
print (1 + 2) * 3
print 10 - 4 + 2
print 20 / 4 / 2

# ===== 字符串操作 =====
print "=== 字符串操作 ==="

# 字符串连接
print "Hello" + " " + "World"
print "数字: " + "42"
print "" + "空字符串连接"

# ===== 比较运算 =====
print "=== 比较运算 ==="

# 相等比较
print 5 == 5
print 3 == 4
print true == true
print false == false
print nil == nil
print "abc" == "abc"
print "abc" == "def"

# 不等比较
print 5 != 3
print 2 != 2
print true != false
print "hello" != "world"

# 大小比较
print 5 > 3
print 2 > 7
print 10 >= 10
print 8 >= 12
print 4 < 9
print 6 < 2
print 7 <= 7
print 15 <= 3

# ===== 逻辑运算 =====
print "=== 逻辑运算 ==="

# 逻辑非
print !true
print !false
print !nil
print !42
print !"hello"

# ===== 一元运算 =====
print "=== 一元运算 ==="

# 负号
print -42
print -(-5)
print -(3 + 4)

# ===== 括号表达式 =====
print "=== 括号表达式 ==="

print (1 + 2)
print (3 * 4) + (5 - 2)
print ((1 + 2) * 3) / 2
print !(true == false)

# ===== 复杂表达式 =====
print "=== 复杂表达式 ==="

# 混合运算
print 1 + 2 * 3 - 4 / 2
print (10 + 5) * 2 - 8 / 4
print !false == true
print 5 > 3 == true
print (2 + 3) == 5

# 字符串和数字混合（在支持的范围内）
print "结果: " + "42"

# ===== 真值测试 =====
print "=== 真值测试 ==="

# 各种值的真假性
print !nil
print !false
print !true
print !0
print !42
print !""
print !"hello"

# ===== 运算符优先级测试 =====
print "=== 运算符优先级测试 ==="

print 2 + 3 * 4        # 应该是 14 (3*4=12, 2+12=14)
print (2 + 3) * 4      # 应该是 20 (2+3=5, 5*4=20)
print 10 - 6 / 2       # 应该是 7 (6/2=3, 10-3=7)
print (10 - 6) / 2     # 应该是 2 (10-6=4, 4/2=2)
print 5 > 3 == true    # 应该是 true (5>3=true, true==true=true)
print 5 > (3 == true)  # 应该是 true (3==true=false, 5>false=true)

# ===== 边界情况测试 =====
print "=== 边界情况测试 ==="

# 零值
print 0
print 0.0
print 0 + 0
print 0 * 100
print 0 / 1

# 负数
print -0
print -1
print -3.14

# 空字符串
print ""
print "" + ""
print "" + "test"

# ===== 嵌套表达式 =====
print "=== 嵌套表达式 ==="

print ((1 + 2) * (3 + 4)) / ((5 - 2) + (6 - 4))
print !(!(true == true))
print (5 > 3) == (2 < 4)
print ((10 + 5) / 3) > ((8 - 2) / 3)

print "=== 语法测试完成 ==="