# MiniScript - 轻量级脚本解释器

一个用C语言实现的轻量级脚本语言解释器，支持JIT编译，语法类似Python，专为嵌入式设备设计。

## 特性

- 类Python语法，易于学习
- JIT编译支持，提升执行性能
- 轻量级设计，适合低ROM设备
- 易于扩展的C API
- 跨平台支持

## 项目结构

```
src/
├── core/           # 核心解释器
├── lexer/          # 词法分析器
├── parser/         # 语法分析器
├── vm/             # 虚拟机
├── jit/            # JIT编译器
├── api/            # C扩展API
└── stdlib/         # 标准库

examples/           # 示例代码
tests/             # 测试用例
```

## 编译

### Linux/Mac
```bash
make
```

### Windows
方法1 - 使用Make (需要安装MinGW/MSYS2):
```cmd
make
```

方法2 - 使用Windows批处理脚本:
```cmd
build_windows.bat
```

### 依赖要求
- GCC编译器
- Windows: MinGW-w64 或 TDM-GCC
- Linux/Mac: 系统默认GCC

## 使用

### Linux/Mac
```c
#include "miniscript.h"

int main() {
    ms_vm_t* vm = ms_vm_new();
    ms_vm_exec_string(vm, "print('Hello, World!')");
    ms_vm_free(vm);
    return 0;
}
```

### Windows
```c
#include "miniscript.h"

int main() {
    ms_vm_t* vm = ms_vm_new();
    ms_vm_exec_string(vm, "print(\"Hello, World!\")");
    ms_vm_free(vm);
    return 0;
}
```

### 运行解释器
```bash
# Linux/Mac
./miniscript examples/hello.ms

# Windows
miniscript.exe examples\hello.ms
```