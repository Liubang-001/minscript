# MiniScript 动态库加载指南

## 概述

MiniScript 支持在运行时动态加载扩展库。这允许你创建独立的扩展模块，无需重新编译核心解释器。

## 工作原理

### 库搜索路径

当使用 `import` 语句导入模块时，MiniScript 会按以下顺序搜索动态库：

1. **可执行文件目录** + 模块名 + 平台特定扩展
   - Windows: `miniscript.exe` 所在目录 + `modulename.dll`
   - Linux: `miniscript` 所在目录 + `modulename.so`
   - macOS: `miniscript` 所在目录 + `modulename.dylib`

2. **可执行文件目录/lib** + 模块名 + 平台特定扩展
   - Windows: `miniscript.exe` 所在目录 + `libmodulename.dll`
   - Linux: `miniscript` 所在目录 + `/libmodulename.so`
   - macOS: `miniscript` 所在目录 + `/libmodulename.dylib`

3. **系统路径**
   - 使用系统的动态库搜索路径

### 编译时配置

可以在编译时指定额外的库搜索路径。修改 `src/vm/vm.c` 中的 `try_load_library()` 函数来添加自定义路径。

## 创建动态库扩展

### 扩展接口

所有动态库扩展必须实现以下两个函数：

```c
// 创建扩展
ms_extension_t* ms_extension_create(void);

// 销毁扩展
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

### 扩展函数签名

所有扩展函数必须遵循以下签名：

```c
typedef ms_value_t (*ms_ext_fn_t)(ms_vm_t* vm, int argc, ms_value_t* args);
```

### 完整示例

参考 `src/ext/example_ext.c` 获取完整的示例实现。

## 编译扩展

### Windows

使用 `build_example_ext.bat` 脚本：

```bash
build_example_ext.bat
```

或手动编译：

```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -shared -o example.dll src/ext/example_ext.c
```

### Linux/macOS

使用 `Makefile.example`：

```bash
make -f Makefile.example
```

或手动编译：

```bash
# Linux
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -shared -o example.so src/ext/example_ext.c

# macOS
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -dynamiclib -o example.dylib src/ext/example_ext.c
```

## 使用动态库

### 基本用法

```miniscript
# 导入动态库
import example

# 调用扩展函数
result = example.greet("World")
print(result)

# 使用别名
import example as ex
value = ex.add(10, 20)
print(value)
```

### 完整示例

参考 `examples/dynamic_import_example.ms` 获取完整的使用示例。

## 部署

### 步骤

1. 编译扩展库
2. 将生成的 `.dll`、`.so` 或 `.dylib` 文件放在 `miniscript.exe` 同一目录
3. 在 MiniScript 脚本中使用 `import` 语句加载

### 目录结构示例

```
project/
├── miniscript.exe
├── example.dll          # 动态库
├── script.ms            # MiniScript 脚本
└── lib/
    └── custom.dll       # 可选：放在 lib 子目录
```

## 高级用法

### 自定义库搜索路径

修改 `src/vm/vm.c` 中的 `try_load_library()` 函数来添加自定义搜索路径：

```c
static ms_dynamic_extension_t* try_load_library(const char* module_name) {
    char lib_path[1024];
    
    // 添加自定义路径
    snprintf(lib_path, sizeof(lib_path), "/custom/path/%s.so", module_name);
    ms_dynamic_extension_t* dyn_ext = ms_load_extension_library(lib_path);
    if (dyn_ext) return dyn_ext;
    
    // ... 其他路径
}
```

### 编译时指定路径

在编译时定义宏来指定库搜索路径：

```bash
gcc -DCUSTOM_LIB_PATH="/opt/miniscript/lib" ...
```

然后在 `src/vm/vm.c` 中使用：

```c
#ifdef CUSTOM_LIB_PATH
    snprintf(lib_path, sizeof(lib_path), "%s/%s.so", CUSTOM_LIB_PATH, module_name);
    // ...
#endif
```

## 故障排除

### 库未找到

- 确保库文件与 `miniscript.exe` 在同一目录
- 检查库文件名是否正确（区分大小写）
- 在 Linux/macOS 上，检查 `LD_LIBRARY_PATH` 环境变量

### 符号未找到

- 确保库导出了 `ms_extension_create` 和 `ms_extension_destroy` 函数
- 检查库是否使用了正确的编译选项（特别是 `-fPIC`）

### 运行时错误

- 检查扩展函数的参数类型和数量
- 确保返回值类型正确
- 查看 MiniScript 的错误消息获取更多信息

## API 参考

### 值类型

```c
typedef enum {
    MS_VAL_BOOL,
    MS_VAL_NIL,
    MS_VAL_INT,
    MS_VAL_FLOAT,
    MS_VAL_STRING,
    MS_VAL_FUNCTION,
    MS_VAL_NATIVE_FUNC,
    MS_VAL_MODULE,
} ms_value_type_t;
```

### 值操作函数

```c
// 创建值
ms_value_t ms_value_bool(bool value);
ms_value_t ms_value_nil(void);
ms_value_t ms_value_int(int64_t value);
ms_value_t ms_value_float(double value);
ms_value_t ms_value_string(const char* value);

// 检查值类型
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

## 最佳实践

1. **错误处理**：始终检查参数数量和类型
2. **内存管理**：正确分配和释放内存
3. **命名约定**：使用清晰的函数名称
4. **文档**：为扩展函数提供文档
5. **测试**：在部署前充分测试扩展

## 许可证

动态库扩展遵循与 MiniScript 相同的许可证。
