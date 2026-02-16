# MiniScript 综合测试
# 结合所有支持的语法特性进行综合测试

print "=== MiniScript 综合功能测试 ==="

# 测试1: 数学计算器
print "测试1: 数学计算器"
print "计算: (10 + 5) * 2 - 8 / 4"
print (10 + 5) * 2 - 8 / 4

print "计算: 3.14 * 2 * 5"
print 3.14 * 2 * 5

# 测试2: 逻辑判断
print "测试2: 逻辑判断"
print "5 > 3 且 2 < 4:"
print 5 > 3
print 2 < 4
print (5 > 3) == (2 < 4)

# 测试3: 字符串处理
print "测试3: 字符串处理"
print "构建问候语:"
print "Hello" + ", " + "MiniScript" + "!"

# 测试4: 真值表
print "测试4: 真值表"
print "true AND true = " + "true"
print true == true
print "true AND false = " + "false"  
print true == false
print "false AND false = " + "true"
print false == false

# 测试5: 数值比较链
print "测试5: 数值比较"
print "比较链: 1 < 2 < 3 的模拟"
print 1 < 2
print 2 < 3
print (1 < 2) == (2 < 3)

# 测试6: 复杂嵌套表达式
print "测试6: 复杂表达式"
print "嵌套计算: ((1+2)*3-4)/((5-2)+1)"
print ((1 + 2) * 3 - 4) / ((5 - 2) + 1)

# 测试7: 边界值测试
print "测试7: 边界值"
print "零值测试:"
print 0
print 0.0
print 0 == 0.0

print "负数测试:"
print -42
print -3.14
print -(-5)

# 测试8: 类型转换和比较
print "测试8: 类型系统"
print "nil 比较:"
print nil == nil
print nil != true
print nil != false

print "布尔值比较:"
print true != false
print !true == false
print !false == true

# 测试9: 运算符优先级验证
print "测试9: 运算符优先级"
print "验证: 2 + 3 * 4 = 14"
print 2 + 3 * 4
print "验证: (2 + 3) * 4 = 20"
print (2 + 3) * 4

# 测试10: 字符串和数值的边界情况
print "测试10: 边界情况"
print "空字符串:"
print ""
print "空字符串连接:"
print "" + "test" + ""

print "数值边界:"
print 1
print -1
print 0

# 测试11: 复杂逻辑表达式
print "测试11: 复杂逻辑"
print "德摩根定律验证: !(A && B) == (!A || !B)"
print "简化版本: !(true == true) == (false == false)"
print !(true == true)
print false == false
print !(true == true) == (false == false)

# 测试12: 性能相关的重复操作
print "测试12: 重复操作（JIT测试）"
print "重复相同计算:"
print 42 + 8
print 42 + 8  
print 42 + 8
print 42 + 8
print 42 + 8

print "=== 综合测试完成 ==="
print "如果所有输出都正确，说明解释器基本功能正常"