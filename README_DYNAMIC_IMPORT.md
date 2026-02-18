# MiniScript 动态库导入功能

## 概述

MiniScript 现已支持**动态库导入**功能，允许在运行时加载扩展模块，无需重新编译核心解释器。这是一个强大的功能，使得用户可以创建独立的扩展库并在脚本中动态加载使用。

## 快速开始（5分钟）

### 1. 编译示例扩展

```bash
# Windows
.\build_example_ext.bat

# Linux/macOS
make -f Makefile.example
```

### 2. 运行示例脚本

```bash
# 方式1：使用示例脚本
miniscript.exe examples/dynamic_import_example.ms

# 方式2：使用测试脚本
miniscript.exe test_dynamic_import.ms
```

### 3. 在自己的脚本中使用

```miniscript
# 导入动态库
import example

# 调用扩展函数
greeting = example.greet("Alice")
print(greeting)

# 使用别名
import example as ex
result = ex.add(10, 20)
print(result)
```

## 工作原理

### 库搜索机制

当执行 `import modulename` 时，MiniScript 按以下顺序搜索库文件：

#### Windows
1. `miniscript.exe` 所在目录 + `modulename.dll`
2. `miniscript.exe` 所在目录 + `libmodulename.dll`
3. 系统路径

#### Linux
1. `miniscript` 所在目录 + `modulename.so`
2. `miniscript` 所在目录 + `libmodulename.so`
3. 系统路径

#### macOS
1. `miniscript` 所在目录 + `modulename.dylib`
2. `miniscript` 所在目录 + `libmodulename.dylib`
3. 系统路径

### 执行流程

```
import example
    ↓
解析 import 语句
    ↓
生成 OP_LOAD_MODULE 字节码
    ↓
运行时执行 OP_LOAD_MODULE
    ↓
调用 try_load_library() 搜索库
    ↓
使用 dlopen/LoadLibrary 加载库
    ↓
调用 ms_extension_create() 初始化
    ↓
注册到全局扩展注册表
    ↓
返回模块值供脚本使用
```

## 创建自定义扩展

### 基本模板

```c
#include "ext.h"
#include <stdlib.h>
#include <string.h>

// 实现扩展函数
static ms_value_t my_func(ms_vm_t* vm, int argc, ms_value_t* args) {
    // 检查参数
    if (argc < 1) {
        return ms_value_nil();
    }
    
    // 处理参数
    if (ms_value_is_int(args[0])) {
        int64_t val = ms_value_as_int(args[0]);
        return ms_value_int(val * 2);
    }
    
    return ms_value_nil();
}

// 创建扩展（必须实现）
ms_extension_t* ms_extension_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "myext";
    ext->function_count = 1;
    ext->functions[0].name = "my_func";
    ext->functions[0].func = my_func;
    return ext;
}

// 销毁扩展（必须实现）
void ms_extension_destroy(ms_extension_t* ext) {
    if (ext) free(ext);
}
```

### 编译扩展

**Windows:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -shared -o myext.dll myext.c -L. -lminiscript
```

**Linux:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -shared -o myext.so myext.c -L. -lminiscript
```

**macOS:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -fPIC -Iinclude -dynamiclib -o myext.dylib myext.c -L. -lminiscript
```

### 部署

1. 编译扩展库
2. 将 `.dll`/`.so`/`.dylib` 文件放在 `miniscript.exe` 同一目录
3. 在脚本中使用 `import myext`

## 示例

### 示例 1: 基本使用

```miniscript
import example

# 调用 greet 函数
msg = example.greet("World")
print(msg)

# 调用 add 函数
sum_val = example.add(10, 20)
print(sum_val)

# 调用 multiply 函数
prod_val = example.multiply(5, 6)
print(prod_val)
```

**输出：**
```
Hello, World!
30
30
```

### 示例 2: 使用别名

```miniscript
import example as ex

result1 = ex.greet("Alice")
result2 = ex.add(100, 200)
result3 = ex.multiply(result2, 2)

print(result1)
print(result2)
print(result3)
```

**输出：**
```
Hello, Alice!
300
600
```

### 示例 3: 条件导入

```miniscript
# 可以在条件语句中使用
if True:
    import example
    result = example.add(5, 10)
    print(result)
```

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

## 故障排除

### 问题：库未找到

**症状：** 运行脚本时出现"模块未找到"错误

**解决方案：**
1. 确保库文件与 `miniscript.exe` 在同一目录
2. 检查库文件名是否正确（区分大小写）
3. 在 Linux/macOS 上，检查 `LD_LIBRARY_PATH` 环境变量

### 问题：符号未找到

**症状：** 编译扩展时出现"undefined symbol"错误

**解决方案：**
1. 确保链接了 `libminiscript`（使用 `-L. -lminiscript`）
2. 检查库是否使用了正确的编译选项（特别是 `-fPIC`）
3. 确保导出了 `ms_extension_create` 和 `ms_extension_destroy` 函数

### 问题：运行时错误

**症状：** 调用扩展函数时出现错误

**解决方案：**
1. 检查参数数量是否正确
2. 检查参数类型是否匹配
3. 确保返回值类型正确
4. 查看 MiniScript 的错误消息获取更多信息

## 文件清单

### 核心实现
- `src/ext/ext.h` - 扩展系统头文件
- `src/ext/ext.c` - 扩展系统实现
- `src/vm/vm.h` - VM 头文件
- `src/vm/vm.c` - VM 实现

### 示例
- `src/ext/example_ext.h` - 示例扩展头文件
- `src/ext/example_ext.c` - 示例扩展实现
- `examples/dynamic_import_example.ms` - 使用示例
- `test_dynamic_import.ms` - 测试脚本

### 构建脚本
- `build_example_ext.bat` - Windows 编译脚本
- `Makefile.example` - Unix 编译脚本

### 文档
- `README_DYNAMIC_IMPORT.md` - 本文件
- `DYNAMIC_IMPORT_GUIDE.md` - 详细用户指南
- `DYNAMIC_LIBRARY_LOADING.md` - 技术文档
- `DYNAMIC_IMPORT_IMPLEMENTATION.md` - 实现总结

## 最佳实践

1. **错误处理** - 始终检查参数数量和类型
2. **内存管理** - 正确分配和释放内存
3. **命名约定** - 使用清晰的函数名称
4. **文档** - 为扩展函数提供文档
5. **测试** - 在部署前充分测试扩展
6. **版本控制** - 跟踪扩展版本

## 编译时配置

### 自定义库搜索路径

修改 `src/vm/vm.c` 中的 `try_load_library()` 函数来添加自定义路径。

### 使用编译时宏

```bash
gcc -DCUSTOM_LIB_PATH="/opt/miniscript/lib" ...
```

## 向后兼容性

✅ 完全向后兼容：
- 现有的内置扩展（http、math、string）继续工作
- 现有的 import 语句继续工作
- 现有的脚本无需修改

## 性能考虑

- 库加载是一次性操作（在 import 时）
- 函数调用性能与内置扩展相同
- 没有额外的运行时开销

## 安全考虑

- 动态库应该来自可信来源
- 建议对库进行代码审查
- 考虑使用沙箱隔离不可信的库

## 许可证

遵循与 MiniScript 相同的许可证。

## 相关文档

- [DYNAMIC_IMPORT_GUIDE.md](DYNAMIC_IMPORT_GUIDE.md) - 详细用户指南
- [DYNAMIC_LIBRARY_LOADING.md](DYNAMIC_LIBRARY_LOADING.md) - 技术文档
- [DYNAMIC_IMPORT_IMPLEMENTATION.md](DYNAMIC_IMPORT_IMPLEMENTATION.md) - 实现总结
- [EXTENSIONS.md](EXTENSIONS.md) - 扩展系统概述
- [EXTENSION_ARCHITECTURE.md](EXTENSION_ARCHITECTURE.md) - 扩展架构

## 支持

如有问题或建议，请参考相关文档或查看示例代码。
