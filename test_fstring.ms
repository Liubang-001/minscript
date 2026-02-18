# Test f-string support

print("=== F-String Tests ===")
print()

# Test 1: Simple f-string
name = "Alice"
msg = f"Hello {name}"
print(msg)
print()

# Test 2: Multiple expressions
age = 25
city = "New York"
result = f"My name is {name}, I am {age} years old, and I live in {city}"
print(result)
print()

# Test 3: Expressions in f-string
x = 10
y = 20
sum_result = f"The sum of {x} and {y} is {x + y}"
print(sum_result)
print()

# Test 4: Arithmetic in f-string
a = 5
b = 3
calc = f"Result: {a * b + 10}"
print(calc)
print()

# Test 5: Boolean in f-string
flag = True
msg2 = f"Flag is {flag}"
print(msg2)
print()

print("=== All F-String Tests Passed ===")
