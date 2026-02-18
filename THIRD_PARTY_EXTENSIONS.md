# Third-Party Extensions for MiniScript

This guide explains how to create third-party extensions for MiniScript that can be compiled separately or linked into the core package.

## Overview

MiniScript supports two types of extensions:

1. **Built-in Extensions** - Compiled into the core miniscript executable
2. **Third-Party Extensions** - Compiled separately and optionally linked

## Built-in Extensions

Built-in extensions are compiled directly into the miniscript executable. Examples include:
- `http` - HTTP client with async I/O
- `math` - Mathematical functions
- `string` - String manipulation

### Registering Built-in Extensions

Built-in extensions are registered in `src/main.c`:

```c
#include "ext/math_ext.h"

int main(int argc, const char* argv[]) {
    ms_vm_t* vm = ms_vm_new();
    
    // Register extension
    ms_extension_t* math_ext = ms_math_extension_create();
    ms_register_extension(vm, math_ext);
    
    // ... run script ...
    
    ms_math_extension_destroy(math_ext);
    ms_vm_free(vm);
    return 0;
}
```

### Building Built-in Extensions

Add to `build_windows.bat`:

```batch
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\ext\math_ext.c -o %BUILD_DIR%\ext\math_ext.o
```

## Creating a Third-Party Extension

### Step 1: Create Extension Header

Create `myext.h`:

```c
#ifndef MYEXT_H
#define MYEXT_H

#include "ext.h"

// Extension API
ms_extension_t* ms_myext_create(void);
void ms_myext_destroy(ms_extension_t* ext);

#endif // MYEXT_H
```

### Step 2: Implement Extension

Create `myext.c`:

```c
#include "myext.h"
#include <stdlib.h>

// Extension function
static ms_value_t my_function(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1) return ms_value_nil();
    
    // Validate arguments
    if (!ms_value_is_int(args[0])) {
        return ms_value_nil();
    }
    
    // Process
    int64_t value = ms_value_as_int(args[0]);
    return ms_value_int(value * 2);
}

// Create extension
ms_extension_t* ms_myext_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "myext";
    ext->function_count = 1;
    
    ext->functions[0].name = "double";
    ext->functions[0].func = my_function;
    
    return ext;
}

// Destroy extension
void ms_myext_destroy(ms_extension_t* ext) {
    if (ext) free(ext);
}
```

### Step 3: Compile Extension

**Option A: Compile as Object File**

```batch
gcc -Wall -Wextra -std=c99 -O2 -Iinclude -c myext.c -o myext.o
```

**Option B: Compile as Shared Library**

```batch
gcc -shared -fPIC -Wall -Wextra -std=c99 -O2 -Iinclude myext.c -o myext.dll
```

### Step 4: Link with MiniScript

**Option A: Link Object File**

```batch
gcc -o miniscript_with_myext miniscript.exe myext.o -lm
```

**Option B: Load Shared Library at Runtime**

```c
#include <dlfcn.h>

typedef ms_extension_t* (*create_fn_t)(void);

// Load extension
void* handle = dlopen("myext.dll", RTLD_LAZY);
create_fn_t create = (create_fn_t)dlsym(handle, "ms_myext_create");
ms_extension_t* ext = create();
ms_register_extension(vm, ext);
```

## Example: Math Extension

The math extension demonstrates a complete third-party extension:

**Files:**
- `src/ext/math_ext.h` - Header
- `src/ext/math_ext.c` - Implementation

**Functions:**
- `math.sqrt(x)` - Square root
- `math.pow(x, y)` - Power
- `math.sin(x)` - Sine
- `math.cos(x)` - Cosine
- `math.abs(x)` - Absolute value
- `math.floor(x)` - Floor
- `math.ceil(x)` - Ceiling

**Usage:**

```python
import math

result = math.sqrt(16)
print(result)  # 4

result = math.pow(2, 8)
print(result)  # 256
```

## Example: String Extension

The string extension demonstrates string manipulation:

**Files:**
- `src/ext/string_ext.h` - Header
- `src/ext/string_ext.c` - Implementation

**Functions:**
- `string.upper(s)` - Convert to uppercase
- `string.lower(s)` - Convert to lowercase
- `string.len(s)` - String length
- `string.reverse(s)` - Reverse string
- `string.startswith(s, prefix)` - Check prefix
- `string.endswith(s, suffix)` - Check suffix
- `string.contains(s, substr)` - Check substring

**Usage:**

```python
import string

text = "Hello World"
result = string.upper(text)
print(result)  # HELLO WORLD

result = string.reverse(text)
print(result)  # dlroW olleH
```

## Extension API Reference

### Creating Values

```c
ms_value_t ms_value_nil(void);
ms_value_t ms_value_bool(bool value);
ms_value_t ms_value_int(int64_t value);
ms_value_t ms_value_float(double value);
ms_value_t ms_value_string(const char* value);
```

### Checking Types

```c
bool ms_value_is_nil(ms_value_t value);
bool ms_value_is_bool(ms_value_t value);
bool ms_value_is_int(ms_value_t value);
bool ms_value_is_float(ms_value_t value);
bool ms_value_is_string(ms_value_t value);
```

### Extracting Values

```c
bool ms_value_as_bool(ms_value_t value);
int64_t ms_value_as_int(ms_value_t value);
double ms_value_as_float(ms_value_t value);
const char* ms_value_as_string(ms_value_t value);
```

## Best Practices

### 1. Argument Validation

Always validate arguments before using them:

```c
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 2) return ms_value_nil();
    
    if (!ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    if (!ms_value_is_int(args[1])) {
        return ms_value_nil();
    }
    
    // Safe to use arguments
    return ms_value_nil();
}
```

### 2. Memory Management

Always free allocated memory:

```c
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    char* buffer = malloc(1024);
    
    // Use buffer...
    
    ms_value_t result = ms_value_string(buffer);
    free(buffer);
    return result;
}
```

### 3. Error Handling

Return nil for errors:

```c
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1) {
        return ms_value_nil();  // Error: missing argument
    }
    
    if (!ms_value_is_int(args[0])) {
        return ms_value_nil();  // Error: wrong type
    }
    
    // Process...
    return ms_value_int(result);
}
```

### 4. Naming Conventions

- Extension names: lowercase (e.g., `math`, `string`, `http`)
- Function names: lowercase with underscores (e.g., `sqrt`, `upper`, `get`)
- File names: `{name}_ext.h` and `{name}_ext.c`

## Distributing Extensions

### Package Structure

```
myext/
├── src/
│   └── myext.c
├── include/
│   └── myext.h
├── examples/
│   └── example.ms
├── README.md
└── LICENSE
```

### README Template

```markdown
# MyExt - MiniScript Extension

Description of your extension.

## Installation

1. Copy `myext.c` and `myext.h` to `src/ext/`
2. Add to `build_windows.bat`:
   ```batch
   %CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\ext\myext.c -o %BUILD_DIR%\ext\myext.o
   ```
3. Register in `src/main.c`:
   ```c
   #include "ext/myext.h"
   ms_extension_t* ext = ms_myext_create();
   ms_register_extension(vm, ext);
   ```

## Usage

```python
import myext

result = myext.function(arg)
print(result)
```

## Functions

- `myext.function(arg)` - Description

## License

MIT License
```

## Testing Extensions

Create test files in `examples/`:

```python
import myext

print("Testing myext extension")
result = myext.function(42)
print(result)
```

Run tests:

```bash
.\miniscript.exe examples/myext_example.ms
```

## Troubleshooting

### Extension Not Loading

1. Check extension is registered in `main.c`
2. Verify extension name matches import statement
3. Check build includes extension files

### Function Not Found

1. Verify function name matches exactly
2. Check function is in extension's function array
3. Ensure function_count is correct

### Memory Leaks

1. Check all malloc calls have corresponding free
2. Use valgrind or similar tool to detect leaks
3. Review extension cleanup functions

## Advanced Topics

### Dynamic Loading

Load extensions at runtime:

```c
#include <dlfcn.h>

void load_extension(ms_vm_t* vm, const char* path) {
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Failed to load: %s\n", dlerror());
        return;
    }
    
    typedef ms_extension_t* (*create_fn_t)(void);
    create_fn_t create = (create_fn_t)dlsym(handle, "ms_ext_create");
    
    if (!create) {
        fprintf(stderr, "Failed to find create function\n");
        dlclose(handle);
        return;
    }
    
    ms_extension_t* ext = create();
    ms_register_extension(vm, ext);
}
```

### Extension Versioning

Add version information:

```c
#define MYEXT_VERSION_MAJOR 1
#define MYEXT_VERSION_MINOR 0
#define MYEXT_VERSION_PATCH 0

static ms_value_t myext_version(ms_vm_t* vm, int argc, ms_value_t* args) {
    char version[32];
    snprintf(version, sizeof(version), "%d.%d.%d",
             MYEXT_VERSION_MAJOR, MYEXT_VERSION_MINOR, MYEXT_VERSION_PATCH);
    return ms_value_string(version);
}
```

## References

- [Extension Architecture](EXTENSION_ARCHITECTURE.md)
- [Extension Guide](EXTENSIONS.md)
- [README Extensions](README_EXTENSIONS.md)
