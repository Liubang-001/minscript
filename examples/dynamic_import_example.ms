# 动态库导入示例
# 这个脚本演示如何使用 import 语句动态加载扩展库

# 导入动态库扩展
import example

# 使用导入的扩展
result1 = example.greet("Alice")
print(result1)

result2 = example.greet("Bob")
print(result2)

# 使用 add 函数
sum_result = example.add(10, 20)
print(sum_result)

# 使用 multiply 函数
mul_result = example.multiply(5, 6)
print(mul_result)

# 可以使用 as 别名
import example as ex

result3 = ex.greet("Charlie")
print(result3)

add_result = ex.add(100, 200)
print(add_result)
