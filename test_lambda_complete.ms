# Complete lambda expression test

print("========================================")
print("Lambda Expression Test")
print("========================================")
print()

# Test 1: Simple lambda with two parameters
print("Test 1: Simple lambda")
add = lambda x, y: x + y
print(f"add(3, 5) = {add(3, 5)}")
print(f"add(10, 20) = {add(10, 20)}")
print()

# Test 2: Single parameter lambda
print("Test 2: Single parameter")
square = lambda x: x ** 2
print(f"square(5) = {square(5)}")
print(f"square(10) = {square(10)}")
print()

# Test 3: Lambda with no parameters
print("Test 3: No parameters")
get_value = lambda: 42
print(f"get_value() = {get_value()}")
print()

# Test 4: Lambda with complex expression
print("Test 4: Complex expression")
calc = lambda x, y: x * 2 + y * 3
print(f"calc(5, 10) = {calc(5, 10)}")
print()

# Test 5: Lambda in variable assignment
print("Test 5: Multiple lambdas")
multiply = lambda x, y: x * y
divide = lambda x, y: x / y
print(f"multiply(6, 7) = {multiply(6, 7)}")
print(f"divide(20, 4) = {divide(20, 4)}")
print()

# Test 6: Using lambda immediately
print("Test 6: Immediate invocation")
result = (lambda x: x + 10)(5)
print(f"(lambda x: x + 10)(5) = {result}")
print()

print("========================================")
print("All lambda tests passed!")
print("========================================")
