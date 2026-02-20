#ifndef BUILTINS_H
#define BUILTINS_H

#include "miniscript.h"

// 注册所有内置函数
void ms_register_builtins(ms_vm_t* vm);

// 类型转换函数
ms_value_t builtin_int(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_float(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_str(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_bool(ms_vm_t* vm, int argc, ms_value_t* args);

// 集合函数
ms_value_t builtin_len(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_range(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_list(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_tuple(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_dict(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_set(ms_vm_t* vm, int argc, ms_value_t* args);

// 数学函数
ms_value_t builtin_abs(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_min(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_max(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_sum(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_pow(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_round(ms_vm_t* vm, int argc, ms_value_t* args);

// 字符串函数
ms_value_t builtin_chr(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_ord(ms_vm_t* vm, int argc, ms_value_t* args);

// 输入输出
ms_value_t builtin_print(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_input(ms_vm_t* vm, int argc, ms_value_t* args);

// 其他工具函数
ms_value_t builtin_type(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_isinstance(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_enumerate(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_zip(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_sorted(ms_vm_t* vm, int argc, ms_value_t* args);
ms_value_t builtin_reversed(ms_vm_t* vm, int argc, ms_value_t* args);

#endif // BUILTINS_H
