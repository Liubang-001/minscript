# Test assert and del statements

print("========================================")
print("Testing assert and del statements")
print("========================================")
print()

# Test 1: assert with true condition
print("Test 1: assert with true condition")
x = 10
assert x == 10
print("assert x == 10 passed")
assert x > 5
print("assert x > 5 passed")
print()

# Test 2: assert with message
print("Test 2: assert with message")
y = 20
assert y < 100, "y should be less than 100"
print("assert y < 100 passed")
print()

# Test 3: del statement
print("Test 3: del statement")
temp = 42
print(f"temp = {temp}")
del temp
print("del temp executed successfully")
print()

# Test 4: Multiple variables
print("Test 4: Multiple variables")
a = 1
b = 2
c = 3
print(f"a = {a}, b = {b}, c = {c}")
del b
print("del b executed")
print(f"a = {a}, c = {c}")
print()

print("========================================")
print("All tests passed!")
print("========================================")
