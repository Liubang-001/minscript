# Comprehensive test for for/while else clause

print("==================================================")
print("FOR...ELSE TESTS")
print("==================================================")

print("\n1. For loop - normal completion (else executes)")
for i in [1, 2, 3]:
    print(f"  Loop: i = {i}")
else:
    print("  ✓ Else block executed")

print("\n2. For loop - with break (else skipped)")
for i in [1, 2, 3, 4, 5]:
    print(f"  Loop: i = {i}")
    if i == 3:
        print("  Breaking at i = 3")
        break
else:
    print("  ✗ This should NOT print")

print("\n3. For loop - empty list (else executes)")
for i in []:
    print(f"  Loop: i = {i}")
else:
    print("  ✓ Else block executed (empty list)")

print("\n4. For loop - nested with else")
for i in [1, 2]:
    print(f"  Outer: i = {i}")
    for j in [10, 20]:
        print(f"    Inner: j = {j}")
    else:
        print("    ✓ Inner else executed")
else:
    print("  ✓ Outer else executed")

print("\n==================================================")
print("WHILE...ELSE TESTS")
print("==================================================")

print("\n5. While loop - normal completion (else executes)")
i = 0
while i < 3:
    print(f"  Loop: i = {i}")
    i = i + 1
else:
    print("  ✓ Else block executed")

print("\n6. While loop - with break (else skipped)")
i = 0
while i < 10:
    print(f"  Loop: i = {i}")
    if i == 2:
        print("  Breaking at i = 2")
        break
    i = i + 1
else:
    print("  ✗ This should NOT print")

print("\n7. While loop - condition false from start (else executes)")
i = 10
while i < 5:
    print(f"  Loop: i = {i}")
    i = i + 1
else:
    print("  ✓ Else block executed (condition false)")

print("\n8. While loop - nested with else")
i = 0
while i < 2:
    print(f"  Outer: i = {i}")
    j = 0
    while j < 2:
        print(f"    Inner: j = {j}")
        j = j + 1
    else:
        print("    ✓ Inner else executed")
    i = i + 1
else:
    print("  ✓ Outer else executed")

print("\n==================================================")
print("MIXED TESTS")
print("==================================================")

print("\n9. For with continue (else executes)")
for i in [1, 2, 3, 4, 5]:
    if i == 3:
        continue
    print(f"  Loop: i = {i}")
else:
    print("  ✓ Else block executed (continue doesn't affect else)")

print("\n10. While with continue (else executes)")
i = 0
while i < 5:
    i = i + 1
    if i == 3:
        continue
    print(f"  Loop: i = {i}")
else:
    print("  ✓ Else block executed (continue doesn't affect else)")

print("\n==================================================")
print("ALL TESTS PASSED!")
print("==================================================")
