print("=== Comprehensive MiniScript Tests ===")

print("1. Basic Variables and Arithmetic")
x = 10
y = 20
z = x + y
print(z)

print("2. String Operations")
name = "MiniScript"
greeting = "Hello, " + name
print(greeting)

print("3. Boolean and Comparisons")
print(True)
print(False)
print(5 > 3)
print(10 == 10)

print("4. Logic Operators")
print(not True)
print(True and False)
print(False or True)

print("5. If/Elif/Else Statements")
score = 85
if score >= 90:
    print("Grade: A")
elif score >= 80:
    print("Grade: B")
elif score >= 70:
    print("Grade: C")
else:
    print("Grade: F")

print("6. Nested If Statements")
a = 10
b = 20
if a > 0:
    if b > 0:
        print("Both positive")
    else:
        print("a positive, b not")
else:
    print("a not positive")

print("7. While Loops")
count = 0
while count < 3:
    print(count)
    count = count + 1

print("=== All Tests Completed ===")
