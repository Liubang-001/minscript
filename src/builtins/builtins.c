#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============ 输入输出函数 ============

// Helper function to print a value recursively
static void print_value(ms_value_t value, int depth) {
    if (depth > 10) {
        printf("...");
        return;
    }
    
    switch (value.type) {
        case MS_VAL_BOOL:
            printf(ms_value_as_bool(value) ? "True" : "False");
            break;
        case MS_VAL_NIL:
            printf("None");
            break;
        case MS_VAL_INT:
            printf("%lld", (long long)ms_value_as_int(value));
            break;
        case MS_VAL_FLOAT:
            printf("%g", ms_value_as_float(value));
            break;
        case MS_VAL_STRING:
            printf("%s", ms_value_as_string(value));
            break;
        case MS_VAL_LIST: {
            ms_list_t* list = ms_value_as_list(value);
            printf("[");
            for (int i = 0; i < ms_list_len(list); i++) {
                if (i > 0) printf(", ");
                print_value(ms_list_get(list, i), depth + 1);
            }
            printf("]");
            break;
        }
        case MS_VAL_TUPLE: {
            ms_tuple_t* tuple = ms_value_as_tuple(value);
            printf("(");
            for (int i = 0; i < ms_tuple_len(tuple); i++) {
                if (i > 0) printf(", ");
                print_value(ms_tuple_get(tuple, i), depth + 1);
            }
            if (ms_tuple_len(tuple) == 1) printf(",");
            printf(")");
            break;
        }
        case MS_VAL_DICT: {
            ms_dict_t* dict = ms_value_as_dict(value);
            printf("{");
            for (int i = 0; i < ms_dict_len(dict); i++) {
                if (i > 0) printf(", ");
                printf("'%s': ", dict->entries[i].key);
                print_value(dict->entries[i].value, depth + 1);
            }
            printf("}");
            break;
        }
        default:
            printf("<object>");
            break;
    }
}

ms_value_t builtin_print(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    for (int i = 0; i < argc; i++) {
        if (i > 0) printf(" ");
        print_value(args[i], 0);
    }
    printf("\n");
    return ms_value_nil();
}

ms_value_t builtin_input(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    // Print prompt if provided
    if (argc > 0 && ms_value_is_string(args[0])) {
        printf("%s", ms_value_as_string(args[0]));
    }
    
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        // Remove trailing newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return ms_value_string(buffer);
    }
    
    return ms_value_string("");
}

// ============ 类型转换函数 ============

ms_value_t builtin_int(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_int(0);
    
    ms_value_t arg = args[0];
    if (ms_value_is_int(arg)) return arg;
    if (ms_value_is_float(arg)) return ms_value_int((int64_t)ms_value_as_float(arg));
    if (ms_value_is_bool(arg)) return ms_value_int(ms_value_as_bool(arg) ? 1 : 0);
    if (ms_value_is_string(arg)) {
        const char* str = ms_value_as_string(arg);
        return ms_value_int(atoll(str));
    }
    return ms_value_int(0);
}

ms_value_t builtin_float(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_float(0.0);
    
    ms_value_t arg = args[0];
    if (ms_value_is_float(arg)) return arg;
    if (ms_value_is_int(arg)) return ms_value_float((double)ms_value_as_int(arg));
    if (ms_value_is_bool(arg)) return ms_value_float(ms_value_as_bool(arg) ? 1.0 : 0.0);
    if (ms_value_is_string(arg)) {
        const char* str = ms_value_as_string(arg);
        return ms_value_float(atof(str));
    }
    return ms_value_float(0.0);
}

ms_value_t builtin_str(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_string("");
    
    ms_value_t arg = args[0];
    char buffer[256];
    
    // TODO: 支持 __str__ 和 __repr__ 魔术方法
    
    if (ms_value_is_string(arg)) return arg;
    if (ms_value_is_int(arg)) {
        snprintf(buffer, sizeof(buffer), "%lld", (long long)ms_value_as_int(arg));
        return ms_value_string(buffer);
    }
    if (ms_value_is_float(arg)) {
        snprintf(buffer, sizeof(buffer), "%g", ms_value_as_float(arg));
        return ms_value_string(buffer);
    }
    if (ms_value_is_bool(arg)) {
        return ms_value_string(ms_value_as_bool(arg) ? "True" : "False");
    }
    if (ms_value_is_nil(arg)) {
        return ms_value_string("None");
    }
    return ms_value_string("<object>");
}

ms_value_t builtin_bool(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_bool(false);
    return ms_value_bool(ms_value_as_bool(args[0]));
}

// ============ 集合函数 ============

ms_value_t builtin_len(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 1) return ms_value_nil();
    
    ms_value_t arg = args[0];
    if (ms_value_is_string(arg)) {
        return ms_value_int(strlen(ms_value_as_string(arg)));
    }
    if (ms_value_is_list(arg)) {
        return ms_value_int(ms_list_len(ms_value_as_list(arg)));
    }
    if (ms_value_is_tuple(arg)) {
        return ms_value_int(ms_tuple_len(ms_value_as_tuple(arg)));
    }
    if (ms_value_is_dict(arg)) {
        return ms_value_int(ms_dict_len(ms_value_as_dict(arg)));
    }
    // TODO: 支持 __len__ 魔术方法
    return ms_value_nil();
}

ms_value_t builtin_range(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    int64_t start = 0, stop = 0, step = 1;
    
    if (argc == 1) {
        stop = ms_value_as_int(args[0]);
    } else if (argc == 2) {
        start = ms_value_as_int(args[0]);
        stop = ms_value_as_int(args[1]);
    } else if (argc == 3) {
        start = ms_value_as_int(args[0]);
        stop = ms_value_as_int(args[1]);
        step = ms_value_as_int(args[2]);
    } else {
        return ms_value_nil();
    }
    
    if (step == 0) return ms_value_nil();
    
    ms_list_t* list = ms_list_new();
    if (step > 0) {
        for (int64_t i = start; i < stop; i += step) {
            ms_list_append(list, ms_value_int(i));
        }
    } else {
        for (int64_t i = start; i > stop; i += step) {
            ms_list_append(list, ms_value_int(i));
        }
    }
    
    return ms_value_list(list);
}

ms_value_t builtin_list(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_list(ms_list_new());
    
    // Convert iterable to list
    ms_value_t arg = args[0];
    ms_list_t* list = ms_list_new();
    
    if (ms_value_is_list(arg)) {
        // Copy list
        ms_list_t* src = ms_value_as_list(arg);
        for (int i = 0; i < ms_list_len(src); i++) {
            ms_list_append(list, ms_list_get(src, i));
        }
    } else if (ms_value_is_tuple(arg)) {
        // Convert tuple to list
        ms_tuple_t* tuple = ms_value_as_tuple(arg);
        for (int i = 0; i < ms_tuple_len(tuple); i++) {
            ms_list_append(list, ms_tuple_get(tuple, i));
        }
    }
    
    return ms_value_list(list);
}

ms_value_t builtin_tuple(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_tuple(ms_tuple_new(0));
    
    // Convert iterable to tuple
    ms_value_t arg = args[0];
    
    if (ms_value_is_tuple(arg)) return arg;
    
    if (ms_value_is_list(arg)) {
        ms_list_t* list = ms_value_as_list(arg);
        int count = ms_list_len(list);
        ms_tuple_t* tuple = ms_tuple_new(count);
        for (int i = 0; i < count; i++) {
            tuple->elements[i] = ms_list_get(list, i);
        }
        return ms_value_tuple(tuple);
    }
    
    return ms_value_tuple(ms_tuple_new(0));
}

ms_value_t builtin_dict(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    (void)argc;
    (void)args;
    return ms_value_dict(ms_dict_new());
}

// ============ 数学函数 ============

ms_value_t builtin_abs(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 1) return ms_value_nil();
    
    ms_value_t arg = args[0];
    if (ms_value_is_int(arg)) {
        int64_t val = ms_value_as_int(arg);
        return ms_value_int(val < 0 ? -val : val);
    }
    if (ms_value_is_float(arg)) {
        double val = ms_value_as_float(arg);
        return ms_value_float(fabs(val));
    }
    return ms_value_nil();
}

ms_value_t builtin_min(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_nil();
    
    ms_value_t min_val = args[0];
    for (int i = 1; i < argc; i++) {
        if (ms_value_is_int(args[i]) && ms_value_is_int(min_val)) {
            if (ms_value_as_int(args[i]) < ms_value_as_int(min_val)) {
                min_val = args[i];
            }
        } else {
            double a = ms_value_as_float(args[i]);
            double b = ms_value_as_float(min_val);
            if (a < b) min_val = args[i];
        }
    }
    return min_val;
}

ms_value_t builtin_max(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_nil();
    
    ms_value_t max_val = args[0];
    for (int i = 1; i < argc; i++) {
        if (ms_value_is_int(args[i]) && ms_value_is_int(max_val)) {
            if (ms_value_as_int(args[i]) > ms_value_as_int(max_val)) {
                max_val = args[i];
            }
        } else {
            double a = ms_value_as_float(args[i]);
            double b = ms_value_as_float(max_val);
            if (a > b) max_val = args[i];
        }
    }
    return max_val;
}

ms_value_t builtin_sum(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_int(0);
    
    ms_value_t iterable = args[0];
    int64_t int_sum = 0;
    double float_sum = 0.0;
    bool has_float = false;
    
    if (ms_value_is_list(iterable)) {
        ms_list_t* list = ms_value_as_list(iterable);
        for (int i = 0; i < ms_list_len(list); i++) {
            ms_value_t val = ms_list_get(list, i);
            if (ms_value_is_float(val)) {
                has_float = true;
                float_sum += ms_value_as_float(val);
            } else {
                int_sum += ms_value_as_int(val);
            }
        }
    }
    
    if (has_float) {
        return ms_value_float(float_sum + int_sum);
    }
    return ms_value_int(int_sum);
}

ms_value_t builtin_pow(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 2) return ms_value_nil();
    
    double base = ms_value_as_float(args[0]);
    double exp = ms_value_as_float(args[1]);
    return ms_value_float(pow(base, exp));
}

ms_value_t builtin_round(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_nil();
    
    double val = ms_value_as_float(args[0]);
    int ndigits = 0;
    
    if (argc > 1) {
        ndigits = (int)ms_value_as_int(args[1]);
    }
    
    if (ndigits == 0) {
        return ms_value_int((int64_t)round(val));
    } else {
        double multiplier = pow(10.0, ndigits);
        return ms_value_float(round(val * multiplier) / multiplier);
    }
}

// ============ 字符串函数 ============

ms_value_t builtin_chr(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 1) return ms_value_nil();
    
    int code = (int)ms_value_as_int(args[0]);
    char str[2] = {(char)code, '\0'};
    return ms_value_string(str);
}

ms_value_t builtin_ord(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 1 || !ms_value_is_string(args[0])) return ms_value_nil();
    
    const char* str = ms_value_as_string(args[0]);
    if (strlen(str) == 0) return ms_value_nil();
    
    return ms_value_int((int64_t)(unsigned char)str[0]);
}

// ============ 其他工具函数 ============

ms_value_t builtin_type(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 1) return ms_value_string("unknown");
    
    ms_value_t arg = args[0];
    switch (arg.type) {
        case MS_VAL_NIL: return ms_value_string("NoneType");
        case MS_VAL_BOOL: return ms_value_string("bool");
        case MS_VAL_INT: return ms_value_string("int");
        case MS_VAL_FLOAT: return ms_value_string("float");
        case MS_VAL_STRING: return ms_value_string("str");
        case MS_VAL_LIST: return ms_value_string("list");
        case MS_VAL_DICT: return ms_value_string("dict");
        case MS_VAL_TUPLE: return ms_value_string("tuple");
        case MS_VAL_FUNCTION: return ms_value_string("function");
        default: return ms_value_string("object");
    }
}

ms_value_t builtin_isinstance(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 2) return ms_value_bool(false);
    
    ms_value_t obj = args[0];
    const char* type_name = ms_value_as_string(args[1]);
    
    if (strcmp(type_name, "int") == 0) return ms_value_bool(ms_value_is_int(obj));
    if (strcmp(type_name, "float") == 0) return ms_value_bool(ms_value_is_float(obj));
    if (strcmp(type_name, "str") == 0) return ms_value_bool(ms_value_is_string(obj));
    if (strcmp(type_name, "bool") == 0) return ms_value_bool(ms_value_is_bool(obj));
    if (strcmp(type_name, "list") == 0) return ms_value_bool(ms_value_is_list(obj));
    if (strcmp(type_name, "dict") == 0) return ms_value_bool(ms_value_is_dict(obj));
    if (strcmp(type_name, "tuple") == 0) return ms_value_bool(ms_value_is_tuple(obj));
    
    return ms_value_bool(false);
}

ms_value_t builtin_enumerate(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0) return ms_value_nil();
    
    ms_value_t iterable = args[0];
    int start = 0;
    if (argc > 1) start = (int)ms_value_as_int(args[1]);
    
    ms_list_t* result = ms_list_new();
    
    if (ms_value_is_list(iterable)) {
        ms_list_t* list = ms_value_as_list(iterable);
        for (int i = 0; i < ms_list_len(list); i++) {
            ms_tuple_t* pair = ms_tuple_new(2);
            pair->elements[0] = ms_value_int(start + i);
            pair->elements[1] = ms_list_get(list, i);
            ms_list_append(result, ms_value_tuple(pair));
        }
    }
    
    return ms_value_list(result);
}

ms_value_t builtin_zip(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 2) return ms_value_nil();
    
    // Find minimum length
    int min_len = -1;
    for (int i = 0; i < argc; i++) {
        if (ms_value_is_list(args[i])) {
            int len = ms_list_len(ms_value_as_list(args[i]));
            if (min_len == -1 || len < min_len) min_len = len;
        }
    }
    
    if (min_len == -1) return ms_value_nil();
    
    ms_list_t* result = ms_list_new();
    for (int i = 0; i < min_len; i++) {
        ms_tuple_t* tuple = ms_tuple_new(argc);
        for (int j = 0; j < argc; j++) {
            if (ms_value_is_list(args[j])) {
                tuple->elements[j] = ms_list_get(ms_value_as_list(args[j]), i);
            }
        }
        ms_list_append(result, ms_value_tuple(tuple));
    }
    
    return ms_value_list(result);
}

ms_value_t builtin_sorted(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0 || !ms_value_is_list(args[0])) return ms_value_nil();
    
    ms_list_t* src = ms_value_as_list(args[0]);
    int len = ms_list_len(src);
    
    // Create a copy
    ms_list_t* result = ms_list_new();
    for (int i = 0; i < len; i++) {
        ms_list_append(result, ms_list_get(src, i));
    }
    
    // Simple bubble sort
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            ms_value_t a = ms_list_get(result, j);
            ms_value_t b = ms_list_get(result, j + 1);
            
            bool should_swap = false;
            if (ms_value_is_int(a) && ms_value_is_int(b)) {
                should_swap = ms_value_as_int(a) > ms_value_as_int(b);
            } else {
                should_swap = ms_value_as_float(a) > ms_value_as_float(b);
            }
            
            if (should_swap) {
                ms_list_set(result, j, b);
                ms_list_set(result, j + 1, a);
            }
        }
    }
    
    return ms_value_list(result);
}

ms_value_t builtin_reversed(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc == 0 || !ms_value_is_list(args[0])) return ms_value_nil();
    
    ms_list_t* src = ms_value_as_list(args[0]);
    int len = ms_list_len(src);
    
    ms_list_t* result = ms_list_new();
    for (int i = len - 1; i >= 0; i--) {
        ms_list_append(result, ms_list_get(src, i));
    }
    
    return ms_value_list(result);
}

// ============ OOP 函数 ============

ms_value_t builtin_super(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    // super() 简化实现：暂时返回 nil
    // 完整实现需要访问调用栈和类层次结构
    (void)argc;
    (void)args;
    return ms_value_nil();
}

// ============ 注册所有内置函数 ============

void ms_register_builtins(ms_vm_t* vm) {
    // 输入输出
    ms_vm_register_function(vm, "print", builtin_print);
    ms_vm_register_function(vm, "input", builtin_input);
    
    // 类型转换
    ms_vm_register_function(vm, "int", builtin_int);
    ms_vm_register_function(vm, "float", builtin_float);
    ms_vm_register_function(vm, "str", builtin_str);
    ms_vm_register_function(vm, "bool", builtin_bool);
    
    // 集合函数
    ms_vm_register_function(vm, "len", builtin_len);
    ms_vm_register_function(vm, "range", builtin_range);
    ms_vm_register_function(vm, "list", builtin_list);
    ms_vm_register_function(vm, "tuple", builtin_tuple);
    ms_vm_register_function(vm, "dict", builtin_dict);
    
    // 数学函数
    ms_vm_register_function(vm, "abs", builtin_abs);
    ms_vm_register_function(vm, "min", builtin_min);
    ms_vm_register_function(vm, "max", builtin_max);
    ms_vm_register_function(vm, "sum", builtin_sum);
    ms_vm_register_function(vm, "pow", builtin_pow);
    ms_vm_register_function(vm, "round", builtin_round);
    
    // 字符串函数
    ms_vm_register_function(vm, "chr", builtin_chr);
    ms_vm_register_function(vm, "ord", builtin_ord);
    
    // 其他工具函数
    ms_vm_register_function(vm, "type", builtin_type);
    ms_vm_register_function(vm, "isinstance", builtin_isinstance);
    ms_vm_register_function(vm, "enumerate", builtin_enumerate);
    ms_vm_register_function(vm, "zip", builtin_zip);
    ms_vm_register_function(vm, "sorted", builtin_sorted);
    ms_vm_register_function(vm, "reversed", builtin_reversed);
    
    // OOP 函数
    ms_vm_register_function(vm, "super", builtin_super);
}
