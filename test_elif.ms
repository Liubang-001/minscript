print("=== Elif Tests ===")

x = 5
if x < 0:
    print("negative")
elif x == 0:
    print("zero")
elif x < 10:
    print("small positive")
else:
    print("large positive")

print("=== Nested If Tests ===")

a = 10
b = 20
if a > 0:
    if b > 0:
        print("both positive")
    else:
        print("a positive, b not")
else:
    print("a not positive")

print("=== Multi-branch Tests ===")

score = 85
if score >= 90:
    print("A")
elif score >= 80:
    print("B")
elif score >= 70:
    print("C")
elif score >= 60:
    print("D")
else:
    print("F")


print("AAAAAAAAAA")