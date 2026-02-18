# MiniScript Extensions - Complete Guide

## Quick Start

### Using Built-in Extensions

```python
# Import HTTP extension
import http

# Import with alias
import os as operating_system

# From imports
from math import sqrt
from collections import defaultdict as dd
```

### Creating a Simple Extension

1. **Create header file** (`src/ext/myext.h`):
```c
#ifndef MYEXT_H
#define MYEXT_H

#include "ext.h"

ms_extension_t* ms_myext_create(void);
void ms_myext_destroy(ms_extension_t* ext);

#endif
```

2. **Implement extension** (`src/ext/myext.c`):
```c
#include "myext.h"
#include <stdlib.h>

static ms_value_t my_function(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1) return ms_value_nil();
    
    // Get first argument
    if (ms_value_is_int(args[0])) {
        int value = ms_value_as_int(args[0]);
        return ms_value_int(value * 2);
    }
    
    return ms_value_nil();
}

ms_extension_t* ms_myext_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "myext";
    ext->function_count = 1;
    
    ext->functions[0].name = "double";
    ext->functions[0].func = my_function;
    
    return ext;
}

void ms_myext_destroy(ms_extension_t* ext) {
    if (ext) free(ext);
}
```

3. **Register in main** (`src/main.c`):
```c
#include "ext/myext.h"

int main(int argc, char* argv[]) {
    ms_vm_t* vm = ms_vm_new();
    
    // Register extension
    ms_extension_t* ext = ms_myext_create();
    ms_register_extension(vm, ext);
    
    // ... run script ...
    
    ms_myext_destroy(ext);
    ms_vm_free(vm);
    return 0;
}
```

4. **Update build script** (`build_windows.bat`):
```batch
%CC% %CFLAGS% -I%INCLUDE_DIR% -c %SRC_DIR%\ext\myext.c -o %BUILD_DIR%\ext\myext.o
```

## HTTP Extension

### Features

- Async HTTP requests
- Cross-platform I/O
- GET, POST, and custom methods

### Usage

```python
import http

# GET request
response = http.get("https://api.example.com/users")

# POST request
response = http.post("https://api.example.com/users", '{"name": "Alice"}')

# Custom request
response = http.request("https://api.example.com/users/1", "DELETE")
```

### Async I/O Models

#### Windows (IOCP)
- Handles 64K+ concurrent connections
- Completion-based model
- Efficient for high-concurrency scenarios

#### Linux (epoll)
- Handles 100K+ concurrent connections
- Event-based model
- Excellent scalability

#### macOS (kqueue)
- Handles 64K+ concurrent connections
- BSD-style event notification
- Efficient for BSD-based systems

## Extension API Reference

### Core Types

```c
// Extension function signature
typedef ms_value_t (*ms_ext_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);

// Extension module
typedef struct {
    const char* name;
    struct {
        const char* name;
        ms_ext_fn_t func;
    } functions[64];
    int function_count;
} ms_extension_t;
```

### Value Functions

```c
// Create values
ms_value_t ms_value_nil(void);
ms_value_t ms_value_bool(bool value);
ms_value_t ms_value_int(int64_t value);
ms_value_t ms_value_float(double value);
ms_value_t ms_value_string(const char* value);

// Check types
bool ms_value_is_nil(ms_value_t value);
bool ms_value_is_bool(ms_value_t value);
bool ms_value_is_int(ms_value_t value);
bool ms_value_is_float(ms_value_t value);
bool ms_value_is_string(ms_value_t value);

// Extract values
bool ms_value_as_bool(ms_value_t value);
int64_t ms_value_as_int(ms_value_t value);
double ms_value_as_float(ms_value_t value);
const char* ms_value_as_string(ms_value_t value);
```

### Extension Functions

```c
// Register extension
void ms_register_extension(ms_vm_t* vm, ms_extension_t* ext);

// Call extension function
ms_value_t ms_call_extension_function(ms_vm_t* vm, const char* module, 
                                      const char* func, int argc, ms_value_t* args);
```

### Event Loop Functions

```c
// Initialize event loop
void ms_http_event_loop_init(void);

// Run event loop
void ms_http_event_loop_run(void);

// Stop event loop
void ms_http_event_loop_stop(void);

// Cleanup event loop
void ms_http_event_loop_cleanup(void);
```

## Best Practices

### 1. Argument Validation

Always validate arguments before using them:

```c
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 2) {
        return ms_value_nil();  // Return nil for invalid args
    }
    
    if (!ms_value_is_string(args[0])) {
        return ms_value_nil();  // Type mismatch
    }
    
    // Safe to use args[0] as string
    const char* str = ms_value_as_string(args[0]);
    
    return ms_value_string(str);
}
```

### 2. Memory Management

Always clean up allocated memory:

```c
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    char* buffer = malloc(1024);
    
    // Use buffer...
    
    free(buffer);  // Always free
    
    return ms_value_string("result");
}
```

### 3. Error Handling

Return appropriate error values:

```c
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1) {
        return ms_value_nil();  // Indicate error
    }
    
    // Process...
    
    return ms_value_int(result);  // Return result
}
```

### 4. Resource Cleanup

Provide cleanup functions:

```c
ms_extension_t* ms_myext_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    // Initialize...
    return ext;
}

void ms_myext_destroy(ms_extension_t* ext) {
    if (ext) {
        // Clean up resources
        free(ext);
    }
}
```

## Performance Tips

1. **Minimize allocations** - Reuse buffers when possible
2. **Use appropriate types** - Use int for integers, not float
3. **Batch operations** - Process multiple items at once
4. **Async I/O** - Use event loop for I/O operations
5. **Profile** - Measure performance before optimizing

## Troubleshooting

### Extension Not Loading

**Problem**: Import statement fails
**Solution**: 
1. Check extension is registered: `ms_register_extension(vm, ext)`
2. Verify extension name matches import
3. Check build includes extension files

### Function Not Found

**Problem**: Function call returns nil
**Solution**:
1. Verify function name matches exactly
2. Check function is in extension's function array
3. Ensure function_count is correct

### Memory Leaks

**Problem**: Program uses increasing memory
**Solution**:
1. Check all malloc calls have corresponding free
2. Use valgrind or similar tool to detect leaks
3. Review extension cleanup functions

### Async I/O Issues

**Problem**: Requests timeout or fail
**Solution**:
1. Check event loop is initialized
2. Verify platform headers are included
3. Check for socket/file descriptor leaks

## Examples

### String Processing Extension

```c
static ms_value_t str_upper(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    char* result = malloc(strlen(str) + 1);
    
    for (int i = 0; str[i]; i++) {
        result[i] = toupper(str[i]);
    }
    result[strlen(str)] = '\0';
    
    ms_value_t ret = ms_value_string(result);
    free(result);
    return ret;
}
```

### Math Extension

```c
static ms_value_t math_sqrt(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1) return ms_value_nil();
    
    double value = 0;
    if (ms_value_is_int(args[0])) {
        value = (double)ms_value_as_int(args[0]);
    } else if (ms_value_is_float(args[0])) {
        value = ms_value_as_float(args[0]);
    } else {
        return ms_value_nil();
    }
    
    return ms_value_float(sqrt(value));
}
```

## References

- [IOCP Documentation](https://docs.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports)
- [epoll Documentation](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [kqueue Documentation](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html)
- [MiniScript Source Code](https://github.com/Liubang-001/minscript)

## Support

For issues, questions, or contributions, please visit the project repository.
