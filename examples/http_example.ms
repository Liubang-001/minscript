print("=== MiniScript HTTP Extension Example ===")
print()

import http

print("1. Basic HTTP Operations")
print("========================")

print("GET request:")
result = http.get("https://api.example.com/users")
print(result)
print()

print("POST request:")
result = http.post("https://api.example.com/users", "data")
print(result)
print()

print("Custom request:")
result = http.request("https://api.example.com/users/1", "DELETE")
print(result)
print()

print("=== Example Complete ===")
