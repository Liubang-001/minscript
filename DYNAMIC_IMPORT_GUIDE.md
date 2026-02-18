# MiniScript 动态导入指南

## 功能概述

MiniScript 现在支持动态库导入，允许在运行时加载扩展模块，无需重新编译核心解释器。

### 主要特性

- ✅ 自动库搜索（可执行文件目录、lib 子目录、系统路径）
- ✅ 跨平台支持（Windows .dll、Linux .so、macOS .dylib）
- ✅ 编译时路径配置
- ✅ 动态库自动注册和清理
- ✅ 标准扩展接口

## 快速开始

### 1. 编译示例扩展

**Windows:**
```bash
.\build_example_ext.bat
```

**Linux/macOS:**
```bash
make -f Makefile.example
```

### 2. 使用动态库

```miniscript
# 导入动态库
import example

# 调用扩展函数
result = example.greet("World")
print(result)

# 使用别名
import example as ex
sum_val = ex.add(10, 20)
print(sum_val)
```

### 3. 运行脚本

```bash
miniscript.exe examples/dynamic_import_example.ms
```

## 库搜索机制

当执行 `import modulename` 时，MiniScript 按以下顺序搜索库文件：

### Windows
1. `miniscript.exe` 所在目录 + `modulename.dll`
2. `miniscript.exe` 所在目录 + `libmodulename.dll`
3. 系统路径中的 `modulename.dll`

### Linux
1. `miniscript` 所在目录 + `modulename.so`
2. `miniscript` 所在目录 + `libmodulename.so`
3. 系统路径中的 `modulename.so`

### macOS
1. `miniscript` 所在目录 + `modulename.dylib`
2. `miniscript` 所在目录 + `libmodulename.dylib`
3. 系统路径中的 `modulename.dylib`

## 创建自定义扩展

### 基本结构

```c
#include "ext.h"

// 扩展函数
static ms_value_t my_function(ms_vm_t* vm, int argc, ms_value_t* args) {
    // 实现函数逻辑
    return ms_value_int(42);
}

// 创建扩展
ms_extension_t* ms_extension_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "myext";
    ext->function_count = 1;
    ext->functions[0].name = "my_function";
    ext->functions[0].func = my_function;
    return ext;
}

// 销毁扩展
void ms_extension_destroy(ms_extension_t* ext) {
    if (ext) free(ext);
}
```

### 编译扩展

**Windows:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -shared -o myext.dll src/ext/myext.c -L. -lminiscript
```

**Linux:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -shared -o myext.so src/ext/myext.c -L. -lminiscript
```

**macOS:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -dynamiclib -o myext.dylib src/ext/myext.c -L. -lminiscript
```

## 部署

### 目录结构

```
project/
├── miniscript.exe          # 解释器
├── example.dll             # 动态库
├── script.ms               # 脚本
└── lib/
    └── custom.dll          # 可选：lib 子目录中的库
```

### 步骤

1. 编译扩展库
2. 将 `.dll`/`.so`/`.dylib` 文件放在 `miniscript.exe` 同一目录
3. 在脚本中使用 `import` 语句

## 编译时配置

### 自定义库搜索路径

修改 `src/vm/vm.c` 中的 `try_load_library()` 函数：

```c
static ms_dynamic_extension_t* try_load_library(const char* module_name) {
    char lib_path[1024];
    
    // 添加自定义路径
    snprintf(lib_path, sizeof(lib_path), "/opt/miniscript/lib/%s.so", module_name);
    ms_dynamic_extension_t* dyn_ext = ms_load_extension_library(lib_path);
    if (dyn_ext) return dyn_ext;
    
    // ... 其他路径
}
```

### 使用编译时宏

```bash
gcc -DCUSTOM_LIB_PATH="/opt/miniscript/lib" ...
```

然后在代码中使用：

```c
#ifdef CUSTOM_LIB_PATH
    snprintf(lib_path, sizeof(lib_path), "%s/%s.so", CUSTOM_LIB_PATH, module_name);
#endif
```

## API 参考

### 扩展接口

```c
// 必须实现的函数
ms_extension_t* ms_extension_create(void);
void ms_extension_destroy(ms_extension_t* ext);
```

### 扩展结构

```c
typedef struct {
    const char* name;                    // 模块名称
    struct {
        const char* name;                // 函数名称
        ms_ext_fn_t func;                // 函数指针
    } functions[64];
    int function_count;                  // 函数数量
} ms_extension_t;
```

### 函数签名

```c
typedef ms_value_t (*ms_ext_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);
```

### 值操作

```c
// 创建值
ms_value_t ms_value_bool(bool value);
ms_value_t ms_value_nil(void);
ms_value_t ms_value_int(int64_t value);
ms_value_t ms_value_float(double value);
ms_value_t ms_value_string(const char* value);

// 检查类型
bool ms_value_is_bool(ms_value_t value);
bool ms_value_is_nil(ms_value_t value);
bool ms_value_is_int(ms_value_t value);
bool ms_value_is_float(ms_value_t value);
bool ms_value_is_string(ms_value_t value);

// 获取值
bool ms_value_as_bool(ms_value_t value);
int64_t ms_value_as_int(ms_value_t value);
double ms_value_as_float(ms_value_t value);
const char* ms_value_as_string(ms_value_t value);
```

## 示例

### 示例 1: 简单的数学扩展

参考 `src/ext/example_ext.c` 获取完整实现。

### 示例 2: 使用动态库

```miniscript
# 导入扩展
import example

# 调用函数
greeting = example.greet("Alice")
print(greeting)

# 计算
result = example.add(10, 20)
print(result)

# 乘法
product = example.multiply(5, 6)
print(product)
```

### 示例 3: 使用别名

```miniscript
import example as math_ext

x = math_ext.add(100, 200)
y = math_ext.multiply(x, 2)
print(y)
```

## 故障排除

### 问题：库未找到

**解决方案：**
- 确保库文件与 `miniscript.exe` 在同一目录
- 检查库文件名是否正确（区分大小写）
- 在 Linux/macOS 上，检查 `LD_LIBRARY_PATH` 环境变量

### 问题：符号未找到

**解决方案：**
- 确保库导出了 `ms_extension_create` 和 `ms_extension_destroy` 函数
- 检查库是否使用了正确的编译选项（特别是 `-fPIC`）
- 确保链接了 `libminiscript`

### 问题：运行时错误

**解决方案：**
- 检查扩展函数的参数类型和数量
- 确保返回值类型正确
- 查看 MiniScript 的错误消息

## 最佳实践

1. **错误处理**：始终检查参数数量和类型
2. **内存管理**：正确分配和释放内存
3. **命名约定**：使用清晰的函数名称
4. **文档**：为扩展函数提供文档
5. **测试**：在部署前充分测试扩展
6. **版本控制**：跟踪扩展版本

## 实现细节

### 动态库加载流程

1. 解析 `import` 语句
2. 生成 `OP_LOAD_MODULE` 字节码
3. 运行时执行 `OP_LOAD_MODULE`
4. 调用 `try_load_library()` 搜索库文件
5. 使用 `ms_load_extension_library()` 加载库
6. 调用 `ms_extension_create()` 初始化扩展
7. 注册扩展到全局注册表
8. 返回模块值

### 清理流程

1. VM 销毁时调用 `ms_vm_free()`
2. 遍历所有动态扩展
3. 调用 `ms_extension_destroy()` 清理扩展
4. 调用 `ms_unload_extension_library()` 卸载库
5. 释放内存

## 文件列表

- `src/ext/ext.h` - 扩展系统头文件（包含动态库 API）
- `src/ext/ext.c` - 扩展系统实现（包含动态库加载）
- `src/ext/example_ext.h` - 示例扩展头文件
- `src/ext/example_ext.c` - 示例扩展实现
- `src/vm/vm.h` - VM 头文件（包含动态扩展跟踪）
- `src/vm/vm.c` - VM 实现（包含库搜索和加载）
- `build_example_ext.bat` - Windows 编译脚本
- `Makefile.example` - Unix 编译脚本
- `examples/dynamic_import_example.ms` - 使用示例
- `DYNAMIC_LIBRARY_LOADING.md` - 详细文档

## 许可证

动态库扩展遵循与 MiniScript 相同的许可证。
