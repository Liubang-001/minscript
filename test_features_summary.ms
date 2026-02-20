# Test Current Implementation Status

print("========================================")
print("MiniScript - Python 3 Features Test")
print("========================================")
print()

# 1. Basic Data Types
print("1. Basic Data Types")
a = 10
b = 3.14
c = "Hello"
d = True
e = None
print(f"Integer: {a}")
print(f"Float: {b}")
print(f"String: {c}")
print(f"Boolean: {d}")
print(f"None: {e}")
print()

# 2. Operators
print("2. Operators")
print(f"Addition: {a + 5}")
print(f"Power: {a ** 2}")
print(f"Floor Division: {a // 3}")
print(f"Modulo: {a % 3}")
print()

# 3. Lists and Slicing
print("3. Lists and Slicing")
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
print("List: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]")
print(f"Slice [2:5]: {numbers[2:5]}")
print(f"Reverse: {numbers[::-1]}")
print()

# 4. List Comprehension
print("4. List Comprehension")
squares = [x**2 for x in range(5)]
print(f"Squares: {squares}")

# With ternary expression
result = ["even" if x % 2 == 0 else "odd" for x in range(5)]
print(f"Even/Odd: {result}")
print()

# 5. Control Flow
print("5. Control Flow")
for i in range(3):
    print(f"  Loop {i}")

score = 85
if score >= 90:
    grade = "A"
elif score >= 80:
    grade = "B"
else:
    grade = "C"
print(f"Grade: {grade}")
print()

# 6. Functions
print("6. Functions")

def add(a, b):
    return a + b

def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

print(f"add(3, 5) = {add(3, 5)}")
print(f"factorial(5) = {factorial(5)}")
print()

# 7. Built-in Functions
print("7. Built-in Functions")
print(f"len([1,2,3]) = {len([1, 2, 3])}")
print(f"sum([1,2,3,4,5]) = {sum([1, 2, 3, 4, 5])}")
print(f"max(3,1,4) = {max(3, 1, 4)}")
print(f"min(3,1,4) = {min(3, 1, 4)}")
print(f"abs(-5) = {abs(-5)}")
print(f"pow(2, 3) = {pow(2, 3)}")
print(f"round(3.14159, 2) = {round(3.14159, 2)}")
print()

# 8. String Operations
print("8. String Operations")
text = "Python"
print(f"String: {text}")
print("Characters:")
for ch in text:
    print(f"  {ch}")
print()

# 9. Nested Structures
print("9. Nested Structures")
matrix = [[1, 2], [3, 4]]
print("Matrix: [[1, 2], [3, 4]]")
print(f"First row: {matrix[0]}")
print(f"Element [0][1]: {matrix[0][1]}")
print()

# 10. Complex Example - Fibonacci
print("10. Complex Example - Fibonacci")

def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

print("First 10 Fibonacci numbers:")
fib_list = []
for i in range(10):
    fib_list = fib_list + [fibonacci(i)]
print(fib_list)
print()

print("========================================")
print("Summary of Implemented Features:")
print("========================================")
print("IMPLEMENTED:")
print("  - Basic data types (int, float, str, bool, None)")
print("  - Lists, tuples, dictionaries")
print("  - All arithmetic operators (+, -, *, /, //, %, **)")
print("  - All comparison operators (==, !=, >, <, >=, <=)")
print("  - Logical operators (and, or, not)")
print("  - if/elif/else statements")
print("  - for and while loops")
print("  - break/continue statements")
print("  - Function definitions (def)")
print("  - Recursion")
print("  - Slicing with full syntax")
print("  - f-string formatting")
print("  - List comprehensions with ternary expressions")
print("  - 26 built-in functions")
print()
print("NOT YET IMPLEMENTED:")
print("  - lambda expressions")
print("  - assert statement")
print("  - del statement")
print("  - for/while else clause")
print("  - Default parameters")
print("  - *args and **kwargs")
print("  - Keyword-only arguments")
print("  - Classes and OOP")
print("  - Exception handling (try/except/finally)")
print("  - Context managers (with)")
print("  - Decorators")
print("  - Generators (yield)")
print("  - Iterators (__iter__, __next__)")
print("  - Async/await")
print("  - global/nonlocal")
print("  - Walrus operator (:=)")
print("  - eval/exec")
print()
print("========================================")
print("Test Complete!")
print("========================================")
