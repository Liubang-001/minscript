# Test for...else clause

print("Test 1: for loop with normal completion (else should execute)")
for i in [1, 2, 3]:
    print(f"  i = {i}")
else:
    print("  else block executed (normal completion)")

print()
print("Test 2: for loop with break (else should NOT execute)")
for j in [1, 2, 3, 4, 5]:
    print(f"  j = {j}")
    if j == 3:
        print("  breaking...")
        break
else:
    print("  else block executed (should not see this)")

print()
print("Test 3: for loop with continue (else should execute)")
for k in [1, 2, 3]:
    if k == 2:
        continue
    print(f"  k = {k}")
else:
    print("  else block executed (normal completion with continue)")

print()
print("All tests completed!")
