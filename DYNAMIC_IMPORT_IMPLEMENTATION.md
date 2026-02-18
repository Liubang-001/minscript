# MiniScript 动态库导入实现总结

## 功能完成

已成功实现 MiniScript 的动态库导入功能，支持在运行时加载扩展库，无需重新编译核心解释器。

## 核心实现

### 1. 扩展系统增强 (`src/ext/ext.h`, `src/ext/ext.c`)

#### 新增数据结构
```c
typedef struct {
    const char* name;
    void* handle;              // 动态库句柄
    ms_extension_t* ext;       // 扩展指针
} ms_dynamic_extension_t;
```

#### 新增 API 函数
- `ms_load_extension_library()` - 加载动态库
- `ms_unload_extension_library()` - 卸载动态库
- `ms_get_extension_from_library()` - 获取扩展

#### 跨平台支持
- Windows: 使用 `LoadLibraryA()` / `GetProcAddress()` / `FreeLibrary()`
- Linux: 使用 `dlopen()` / `dlsym()` / `dlclose()`
- macOS: 使用 `dlopen()` / `dlsym()` / `dlclose()`

### 2. VM 增强 (`src/vm/vm.h`, `src/vm/vm.c`)

#### VM 结构扩展
```c
struct ms_vm {
    // ... 现有字段 ...
    void* dynamic_extensions[32];      // 动态扩展数组
    int dynamic_extension_count;       // 动态扩展计数
};
```

#### 库搜索机制
在 `src/vm/vm.c` 中实现 `try_load_library()` 函数，按以下顺序搜索库：
1. 可执行文件目录 + 模块名 + 平台扩展
2. 可执行文件目录/lib + 模块名 + 平台扩展
3. 系统路径

#### OP_LOAD_MODULE 增强
- 尝试加载动态库
- 自动注册扩展
- 跟踪动态扩展以便后续清理

#### 清理机制
- `ms_vm_free()` 遍历所有动态扩展
- 调用 `ms_extension_destroy()` 清理
- 调用 `ms_unload_extension_library()` 卸载库

### 3. 构建系统更新

#### Makefile
- 添加 `-ldl` 链接标志（Linux/macOS）

#### build_windows.bat
- 已支持编译（无需修改）

### 4. 示例实现

#### 示例扩展 (`src/ext/example_ext.c`)
实现了三个示例函数：
- `greet(name)` - 返回问候消息
- `add(a, b)` - 返回两数之和
- `multiply(a, b)` - 返回两数之积

#### 编译脚本
- `build_example_ext.bat` - Windows 编译
- `Makefile.example` - Unix 编译

## 库搜索路径

### Windows
```
1. C:\path\to\miniscript.exe 目录 + example.dll
2. C:\path\to\miniscript.exe 目录 + libexample.dll
3. 系统路径中的 example.dll
```

### Linux
```
1. /path/to/miniscript 目录 + example.so
2. /path/to/miniscript 目录 + libexample.so
3. 系统路径中的 example.so
```

### macOS
```
1. /path/to/miniscript 目录 + example.dylib
2. /path/to/miniscript 目录 + libexample.dylib
3. 系统路径中的 example.dylib
```

## 使用示例

### 编译扩展

**Windows:**
```bash
.\build_example_ext.bat
```

**Linux/macOS:**
```bash
make -f Makefile.example
```

### 使用扩展

```miniscript
# 导入动态库
import example

# 调用函数
result = example.greet("World")
print(result)

# 使用别名
import example as ex
sum_val = ex.add(10, 20)
print(sum_val)
```

### 运行脚本

```bash
miniscript.exe examples/dynamic_import_example.ms
miniscript.exe test_dynamic_import.ms
```

## 测试结果

✅ 所有测试通过：
- 基本导入功能
- 函数调用
- 参数传递
- 返回值处理
- 别名支持
- 多个操作

## 文件清单

### 核心实现
- `src/ext/ext.h` - 扩展系统头文件（新增动态库 API）
- `src/ext/ext.c` - 扩展系统实现（新增动态库加载）
- `src/vm/vm.h` - VM 头文件（新增动态扩展跟踪）
- `src/vm/vm.c` - VM 实现（新增库搜索和加载）

### 示例
- `src/ext/example_ext.h` - 示例扩展头文件
- `src/ext/example_ext.c` - 示例扩展实现
- `examples/dynamic_import_example.ms` - 使用示例
- `test_dynamic_import.ms` - 测试脚本

### 构建脚本
- `build_example_ext.bat` - Windows 编译脚本
- `Makefile.example` - Unix 编译脚本
- `Makefile` - 更新（添加 -ldl）

### 文档
- `DYNAMIC_LIBRARY_LOADING.md` - 详细技术文档
- `DYNAMIC_IMPORT_GUIDE.md` - 用户指南
- `DYNAMIC_IMPORT_IMPLEMENTATION.md` - 实现总结（本文件）

## 编译时配置

### 自定义库搜索路径

修改 `src/vm/vm.c` 中的 `try_load_library()` 函数来添加自定义路径。

### 使用编译时宏

```bash
gcc -DCUSTOM_LIB_PATH="/opt/miniscript/lib" ...
```

## 部署指南

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

## 技术亮点

1. **跨平台支持** - 自动适配 Windows/Linux/macOS
2. **自动库搜索** - 智能搜索多个路径
3. **自动清理** - VM 销毁时自动卸载库
4. **标准接口** - 统一的扩展创建/销毁接口
5. **零侵入** - 不影响现有的内置扩展

## 向后兼容性

✅ 完全向后兼容：
- 现有的内置扩展（http、math、string）继续工作
- 现有的 import 语句继续工作
- 现有的脚本无需修改

## 下一步

可选的增强功能：
1. 扩展版本管理
2. 扩展依赖解析
3. 扩展热重载
4. 扩展沙箱隔离
5. 扩展包管理器

## 许可证

遵循与 MiniScript 相同的许可证。
