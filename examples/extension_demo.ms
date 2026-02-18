print("=== MiniScript Extension System Demo ===")
print()

print("1. Importing Extensions")
print("======================")

import http
print("HTTP extension imported successfully")

import os
print("OS extension imported successfully")

from math import sqrt
print("Math module imported")

from collections import defaultdict as dd
print("Collections module imported")

print()
print("2. Using With Statement")
print("=======================")

x = 100
with x as value:
    print("Value in with block:")
    print(value)

print()
print("3. Extension System Features")
print("============================")

print("- Modular architecture")
print("- Cross-platform async I/O")
print("- Windows IOCP support")
print("- Linux epoll support")
print("- macOS kqueue support")

print()
print("=== Demo Complete ===")
