# MiniScript 扩展系统

## 概述

MiniScript 提供了一个灵活的扩展系统，允许开发者创建和集成自定义功能。系统支持跨平台的异步I/O操作。

## 架构

### 核心组件

1. **扩展框架** (`src/ext/ext.h`, `src/ext/ext.c`)
   - 扩展注册表
   - 扩展函数调用机制
   - 模块管理

2. **HTTP扩展** (`src/ext/http.h`, `src/ext/http.c`)
   - 异步HTTP请求
   - 跨平台I/O支持

### 异步I/O支持

MiniScript HTTP扩展支持多种异步I/O模型：

- **Windows**: IOCP (I/O Completion Ports)
- **Linux**: epoll
- **macOS**: kqueue

## 使用示例

### 基本导入

```python
import http

# HTTP扩展已加载
print("HTTP extension ready")
```

### HTTP请求

```python
import http

# GET请求
response = http.get("https://api.example.com/data")
print(response)

# POST请求
response = http.post("https://api.example.com/data", '{"key": "value"}')
print(response)

# 自定义请求
response = http.request("https://api.example.com/data", "PUT")
print(response)
```

## 创建自定义扩展

### 步骤1: 定义扩展头文件

```c
// src/ext/myext.h
#ifndef MYEXT_H
#define MYEXT_H

#include "ext.h"

ms_extension_t* ms_myext_create(void);
void ms_myext_destroy(ms_extension_t* ext);

#endif
```

### 步骤2: 实现扩展

```c
// src/ext/myext.c
#include "myext.h"
#include <stdlib.h>

static ms_value_t my_function(ms_vm_t* vm, int argc, ms_value_t* args) {
    // 实现你的函数
    return ms_value_int(42);
}

ms_extension_t* ms_myext_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "myext";
    ext->function_count = 1;
    
    ext->functions[0].name = "my_function";
    ext->functions[0].func = my_function;
    
    return ext;
}

void ms_myext_destroy(ms_extension_t* ext) {
    if (ext) free(ext);
}
```

### 步骤3: 注册扩展

在 `src/main.c` 中：

```c
#include "ext/ext.h"
#include "ext/http.h"
#include "ext/myext.h"

int main(int argc, char* argv[]) {
    ms_vm_t* vm = ms_vm_new();
    
    // 注册HTTP扩展
    ms_extension_t* http_ext = ms_http_extension_create();
    ms_register_extension(vm, http_ext);
    
    // 注册自定义扩展
    ms_extension_t* my_ext = ms_myext_create();
    ms_register_extension(vm, my_ext);
    
    // ... 运行脚本 ...
    
    ms_http_extension_destroy(http_ext);
    ms_myext_destroy(my_ext);
    ms_vm_free(vm);
    
    return 0;
}
```

## 异步I/O事件循环

### 初始化

```c
ms_http_event_loop_init();
```

### 运行

```c
ms_http_event_loop_run();
```

### 停止

```c
ms_http_event_loop_stop();
```

### 清理

```c
ms_http_event_loop_cleanup();
```

## 扩展API参考

### 扩展函数签名

```c
typedef ms_value_t (*ms_ext_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);
```

### 参数

- `vm`: 虚拟机指针
- `argc`: 参数个数
- `args`: 参数数组

### 返回值

返回 `ms_value_t` 类型的值

## 性能考虑

1. **异步I/O**: 使用平台特定的高效I/O模型
2. **事件驱动**: 基于事件的处理避免阻塞
3. **可扩展性**: 支持大量并发连接

## 平台支持

| 平台 | I/O模型 | 状态 |
|------|--------|------|
| Windows | IOCP | ✓ 支持 |
| Linux | epoll | ✓ 支持 |
| macOS | kqueue | ✓ 支持 |

## 限制

- 最多支持32个扩展模块
- 每个模块最多64个函数
- 最多256个并发异步请求

## 未来改进

- [ ] 更多内置扩展（数据库、文件系统等）
- [ ] 扩展热加载
- [ ] 扩展版本管理
- [ ] 扩展依赖解析
