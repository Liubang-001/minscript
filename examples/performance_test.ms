# MiniScript 性能测试
# 测试JIT编译器的热点检测

print "=== 性能测试 ==="

# 重复计算以触发JIT编译
print "重复计算测试（可能触发JIT）:"

# 简单算术运算
print 1 + 1
print 1 + 1
print 1 + 1
print 1 + 1
print 1 + 1

# 复杂表达式
print (2 + 3) * 4
print (2 + 3) * 4
print (2 + 3) * 4
print (2 + 3) * 4
print (2 + 3) * 4

# 字符串操作
print "Hello" + " World"
print "Hello" + " World"
print "Hello" + " World"
print "Hello" + " World"
print "Hello" + " World"

print "性能测试完成"