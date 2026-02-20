print("Test: List comp with expression")
result = [x * 2 for x in [1, 2, 3]]
print(f"result = {result}")
print(f"len(result) = {len(result)}")
if len(result) > 0:
    print(f"result[0] = {result[0]}")
    print(f"result[1] = {result[1]}")
    print(f"result[2] = {result[2]}")
print("Done")
