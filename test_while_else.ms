# Test while...else clause

print("Test 1: while loop with normal completion (else should execute)")
i = 0
while i < 3:
    print(f"  i = {i}")
    i = i + 1
else:
    print("  else block executed (normal completion)")

print()
print("Test 2: while loop with break (else should NOT execute)")
j = 0
while j < 10:
    print(f"  j = {j}")
    if j == 2:
        print("  breaking...")
        break
    j = j + 1
else:
    print("  else block executed (should not see this)")

print()
print("Test 3: while loop with continue (else should execute)")
k = 0
while k < 3:
    k = k + 1
    if k == 2:
        continue
    print(f"  k = {k}")
else:
    print("  else block executed (normal completion with continue)")

print()
print("All tests completed!")
