numbers = [1, 2, 3]
result_list = ["偶数" if n % 2 == 0 else "奇数" for n in numbers]

print(f"len(numbers) = {len(numbers)}, len(result_list) = {len(result_list)}")

print("Before for loop")
r = range(len(numbers))
print(f"range(len(numbers)) = {r}")
for i in r:
    print(f"  {numbers[i]} -> {result_list[i]}")
print("After for loop")
