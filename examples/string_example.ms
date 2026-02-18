print("=== MiniScript String Extension Example ===")
print()

import string

print("1. Upper and Lower Case")
print("=======================")
text = "Hello World"
result = string.upper(text)
print(result)

result = string.lower(text)
print(result)
print()

print("2. String Length")
print("================")
text = "MiniScript"
result = string.len(text)
print(result)
print()

print("3. Reverse String")
print("=================")
text = "Python"
result = string.reverse(text)
print(result)
print()

print("4. String Checking")
print("==================")
text = "Hello World"

result = string.startswith(text, "Hello")
print(result)

result = string.endswith(text, "World")
print(result)

result = string.contains(text, "World")
print(result)
print()

print("=== String Extension Demo Complete ===")
