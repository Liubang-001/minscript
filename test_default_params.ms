# Test default parameters

print("Test 1: Function with one default parameter")
def greet(name, greeting="Hello"):
    print(f"{greeting}, {name}!")

greet("Alice")
greet("Bob", "Hi")

print()
print("Test 2: Function with multiple default parameters")
def create_user(name, age=18, city="Unknown"):
    print(f"Name: {name}, Age: {age}, City: {city}")

create_user("Alice")
create_user("Bob", 25)
create_user("Charlie", 30, "New York")

print()
print("Test 3: Function with all default parameters")
def config(host="localhost", port=8080, debug=True):
    print(f"Host: {host}, Port: {port}, Debug: {debug}")

config()
config("example.com")
config("example.com", 3000)
config("example.com", 3000, False)

print()
print("Test 4: Default parameter with different types")
def process(data, multiplier=2, prefix="Result"):
    result = data * multiplier
    print(f"{prefix}: {result}")

process(5)
process(5, 3)
process(5, 3, "Output")

print()
print("Test 5: Default parameter with None")
def find(item, default=None):
    if default == None:
        print(f"Searching for {item} (no default)")
    else:
        print(f"Searching for {item} (default: {default})")

find("key")
find("key", "not found")

print()
print("All tests completed!")
