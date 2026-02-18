#include "vm.h"
#include "../ext/ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
    #include <windows.h>
    #define PATH_SEPARATOR "\\"
#else
    #include <unistd.h>
    #define PATH_SEPARATOR "/"
#endif

// 外部名称表
char* name_table_names[256];
int name_table_count = 0;

// 获取可执行文件所在目录
static void get_exe_directory(char* buffer, size_t size) {
#ifdef _WIN32
    GetModuleFileNameA(NULL, buffer, size);
    char* last_sep = strrchr(buffer, '\\');
    if (last_sep) {
        *(last_sep + 1) = '\0';
    }
#else
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        char* last_sep = strrchr(path, '/');
        if (last_sep) {
            *last_sep = '\0';
        }
        strncpy(buffer, path, size - 1);
        buffer[size - 1] = '\0';
    } else {
        buffer[0] = '.';
        buffer[1] = '\0';
    }
#endif
}

// 尝试从多个路径加载动态库
static ms_dynamic_extension_t* try_load_library(const char* module_name) {
    char lib_path[1024];
    char exe_dir[1024];
    
    // 获取可执行文件目录
    get_exe_directory(exe_dir, sizeof(exe_dir));
    
    // 尝试路径1: 当前目录 + 模块名 + .dll/.so/.dylib
#ifdef _WIN32
    snprintf(lib_path, sizeof(lib_path), "%s%s.dll", exe_dir, module_name);
#elif __APPLE__
    snprintf(lib_path, sizeof(lib_path), "%s/%s.dylib", exe_dir, module_name);
#else
    snprintf(lib_path, sizeof(lib_path), "%s/%s.so", exe_dir, module_name);
#endif
    
    ms_dynamic_extension_t* dyn_ext = ms_load_extension_library(lib_path);
    if (dyn_ext) return dyn_ext;
    
    // 尝试路径2: 当前目录 + lib + 模块名 + .dll/.so/.dylib
#ifdef _WIN32
    snprintf(lib_path, sizeof(lib_path), "%slib%s.dll", exe_dir, module_name);
#elif __APPLE__
    snprintf(lib_path, sizeof(lib_path), "%s/lib%s.dylib", exe_dir, module_name);
#else
    snprintf(lib_path, sizeof(lib_path), "%s/lib%s.so", exe_dir, module_name);
#endif
    
    dyn_ext = ms_load_extension_library(lib_path);
    if (dyn_ext) return dyn_ext;
    
    // 尝试路径3: 直接使用模块名（系统路径）
    dyn_ext = ms_load_extension_library(module_name);
    if (dyn_ext) return dyn_ext;
    
    return NULL;
}

static void runtime_error(ms_vm_t* vm, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(vm->error_message, sizeof(vm->error_message), format, args);
    va_end(args);
    vm->has_error = true;
}

static ms_value_t peek(ms_vm_t* vm, int distance) {
    return vm->stack_top[-1 - distance];
}

static bool is_falsey(ms_value_t value) {
    return ms_value_is_nil(value) || 
           (ms_value_is_bool(value) && !ms_value_as_bool(value));
}

static bool values_equal(ms_value_t a, ms_value_t b) {
    if (a.type != b.type) return false;
    
    switch (a.type) {
        case MS_VAL_BOOL: return a.as.boolean == b.as.boolean;
        case MS_VAL_NIL: return true;
        case MS_VAL_INT: return a.as.integer == b.as.integer;
        case MS_VAL_FLOAT: return a.as.floating == b.as.floating;
        case MS_VAL_STRING: return strcmp(a.as.string, b.as.string) == 0;
        default: return false;
    }
}

static ms_result_t run(ms_vm_t* vm) {
    ms_call_frame_t* frame = &vm->frames[vm->frame_count - 1];
    
#define READ_BYTE() (*frame->ip++)
#define READ_CONSTANT() (vm->chunk->constants[READ_BYTE()])
#define READ_SHORT() \
    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define BINARY_OP(value_type, op) \
    do { \
        ms_value_t b = peek(vm, 0); \
        ms_value_t a = peek(vm, 1); \
        if (ms_value_is_int(a) && ms_value_is_int(b)) { \
            ms_vm_pop(vm); \
            ms_vm_pop(vm); \
            ms_vm_push(vm, value_type(ms_value_as_int(a) op ms_value_as_int(b))); \
        } else if ((ms_value_is_int(a) || ms_value_is_float(a)) && \
                   (ms_value_is_int(b) || ms_value_is_float(b))) { \
            ms_vm_pop(vm); \
            ms_vm_pop(vm); \
            ms_vm_push(vm, ms_value_float(ms_value_as_float(a) op ms_value_as_float(b))); \
        } else { \
            runtime_error(vm, "Operands must be numbers."); \
            return MS_RESULT_RUNTIME_ERROR; \
        } \
    } while (false)

    for (;;) {
        uint8_t instruction = READ_BYTE();
        switch (instruction) {
            case OP_CONSTANT: {
                ms_value_t constant = READ_CONSTANT();
                ms_vm_push(vm, constant);
                break;
            }
            case OP_NIL: ms_vm_push(vm, ms_value_nil()); break;
            case OP_TRUE: ms_vm_push(vm, ms_value_bool(true)); break;
            case OP_FALSE: ms_vm_push(vm, ms_value_bool(false)); break;
            case OP_POP: ms_vm_pop(vm); break;
            case OP_GET_LOCAL: {
                uint8_t slot = READ_BYTE();
                ms_vm_push(vm, frame->slots[slot]);
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                frame->slots[slot] = peek(vm, 0);
                break;
            }
            case OP_GET_GLOBAL: {
                uint8_t name_index = READ_BYTE();
                if (name_index >= name_table_count) {
                    runtime_error(vm, "Undefined variable.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                char* name = name_table_names[name_index];
                ms_global_t* current = vm->globals;
                while (current != NULL) {
                    if (strcmp(current->name, name) == 0) {
                        ms_vm_push(vm, current->value);
                        goto found_global;
                    }
                    current = current->next;
                }
                
                runtime_error(vm, "Undefined variable '%s'.", name);
                return MS_RESULT_RUNTIME_ERROR;
                
            found_global:
                break;
            }
            case OP_DEFINE_GLOBAL: {
                uint8_t name_index = READ_BYTE();
                if (name_index >= name_table_count) {
                    runtime_error(vm, "Invalid variable name index.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                char* name = name_table_names[name_index];
                ms_vm_set_global(vm, name, peek(vm, 0));
                break;
            }
            case OP_SET_GLOBAL: {
                uint8_t name_index = READ_BYTE();
                if (name_index >= name_table_count) {
                    runtime_error(vm, "Invalid variable name index.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                char* name = name_table_names[name_index];
                
                // 检查变量是否存在
                ms_global_t* current = vm->globals;
                while (current != NULL) {
                    if (strcmp(current->name, name) == 0) {
                        current->value = peek(vm, 0);
                        goto set_global_done;
                    }
                    current = current->next;
                }
                
                runtime_error(vm, "Undefined variable '%s'.", name);
                return MS_RESULT_RUNTIME_ERROR;
                
            set_global_done:
                break;
            }
            case OP_EQUAL: {
                ms_value_t b = ms_vm_pop(vm);
                ms_value_t a = ms_vm_pop(vm);
                ms_vm_push(vm, ms_value_bool(values_equal(a, b)));
                break;
            }
            case OP_GREATER: BINARY_OP(ms_value_bool, >); break;
            case OP_LESS: BINARY_OP(ms_value_bool, <); break;
            case OP_ADD: {
                if (ms_value_is_string(peek(vm, 0)) && ms_value_is_string(peek(vm, 1))) {
                    // 字符串连接
                    const char* b = ms_value_as_string(ms_vm_pop(vm));
                    const char* a = ms_value_as_string(ms_vm_pop(vm));
                    char* result = malloc(strlen(a) + strlen(b) + 1);
                    strcpy(result, a);
                    strcat(result, b);
                    ms_vm_push(vm, ms_value_string(result));
                    free(result);
                } else if (ms_value_is_int(peek(vm, 0)) && ms_value_is_int(peek(vm, 1))) {
                    BINARY_OP(ms_value_int, +);
                } else {
                    runtime_error(vm, "Operands must be two numbers or two strings.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: BINARY_OP(ms_value_int, -); break;
            case OP_MULTIPLY: BINARY_OP(ms_value_int, *); break;
            case OP_DIVIDE: BINARY_OP(ms_value_int, /); break;
            case OP_NOT:
                ms_vm_push(vm, ms_value_bool(is_falsey(ms_vm_pop(vm))));
                break;
            case OP_NEGATE: {
                if (!ms_value_is_int(peek(vm, 0))) {
                    runtime_error(vm, "Operand must be a number.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                ms_vm_push(vm, ms_value_int(-ms_value_as_int(ms_vm_pop(vm))));
                break;
            }
            case OP_PRINT: {
                ms_value_t value = ms_vm_pop(vm);
                switch (value.type) {
                    case MS_VAL_BOOL:
                        printf(ms_value_as_bool(value) ? "True" : "False");
                        break;
                    case MS_VAL_NIL: printf("None"); break;
                    case MS_VAL_INT: printf("%lld", ms_value_as_int(value)); break;
                    case MS_VAL_FLOAT: printf("%g", ms_value_as_float(value)); break;
                    case MS_VAL_STRING: printf("%s", ms_value_as_string(value)); break;
                    default: printf("<object>"); break;
                }
                printf("\n");
                break;
            }
            case OP_JUMP: {
                uint16_t offset = READ_SHORT();
                frame->ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint16_t offset = READ_SHORT();
                if (is_falsey(peek(vm, 0))) frame->ip += offset;
                break;
            }
            case OP_JUMP_IF_TRUE: {
                uint16_t offset = READ_SHORT();
                if (!is_falsey(peek(vm, 0))) frame->ip += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = READ_SHORT();
                frame->ip -= offset;
                break;
            }
            case OP_CALL: {
                uint8_t arg_count = READ_BYTE();
                ms_value_t func_val = peek(vm, arg_count);
                
                // Check if this is a module method call
                if (func_val.type == MS_VAL_MODULE && vm->last_method_name != NULL) {
                    // Call extension method
                    const char* module_name = (const char*)func_val.as.module;
                    const char* method_name = vm->last_method_name;
                    
                    ms_value_t* args = vm->stack_top - arg_count;
                    
                    // Call the extension function
                    ms_value_t result = ms_call_extension_function(vm, module_name, method_name, arg_count, args);
                    
                    vm->stack_top -= arg_count + 1;
                    ms_vm_push(vm, result);
                    
                    vm->last_method_name = NULL;
                    vm->last_module_name = NULL;
                } else if (func_val.type == MS_VAL_NATIVE_FUNC && func_val.as.native_func != NULL) {
                    // 原生函数调用
                    ms_value_t* args = vm->stack_top - arg_count;
                    ms_value_t result = func_val.as.native_func->func(vm, arg_count, args);
                    vm->stack_top -= arg_count + 1;
                    ms_vm_push(vm, result);
                } else if (func_val.type == MS_VAL_FUNCTION) {
                    // 用户定义的函数调用
                    ms_function_t* function = func_val.as.function;
                    
                    if (arg_count != function->arity) {
                        runtime_error(vm, "Expected %d arguments but got %d.", function->arity, arg_count);
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    
                    if (vm->frame_count >= 64) {
                        runtime_error(vm, "Stack overflow.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    
                    // 保存栈指针位置（函数和参数之前）
                    ms_value_t* call_stack_base = vm->stack_top - arg_count - 1;
                    
                    // 创建新的调用帧
                    ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                    new_frame->ip = function->chunk->code;
                    new_frame->slots = vm->stack_top - arg_count;
                    
                    // 保存当前的chunk
                    ms_chunk_t* prev_chunk = vm->chunk;
                    vm->chunk = function->chunk;
                    
                    // 执行函数
                    ms_result_t result = run(vm);
                    
                    // 恢复chunk
                    vm->chunk = prev_chunk;
                    vm->frame_count--;
                    
                    if (result != MS_RESULT_OK) {
                        return result;
                    }
                    
                    // 函数返回值应该在栈上，清理函数和参数
                    ms_value_t return_value = ms_vm_pop(vm);
                    vm->stack_top = call_stack_base;  // 恢复栈指针到函数调用前
                    ms_vm_push(vm, return_value);      // 推送返回值
                } else {
                    runtime_error(vm, "Can only call functions.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_RETURN: {
                return MS_RESULT_OK;
            }
            case OP_GET_PROPERTY: {
                uint8_t name_index = READ_BYTE();
                if (name_index >= name_table_count) {
                    runtime_error(vm, "Invalid property name index.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_value_t obj = ms_vm_pop(vm);
                const char* prop_name = name_table_names[name_index];
                
                // If it's a module, store the method name for the call handler
                if (obj.type == MS_VAL_MODULE) {
                    vm->last_module_name = (const char*)obj.as.module;
                    vm->last_method_name = prop_name;
                    ms_vm_push(vm, obj);
                } else {
                    // For other types, just return nil
                    vm->last_method_name = NULL;
                    vm->last_module_name = NULL;
                    ms_vm_push(vm, ms_value_nil());
                }
                break;
            }
            case OP_LOAD_MODULE: {
                uint8_t module_index = READ_BYTE();
                if (module_index >= name_table_count) {
                    runtime_error(vm, "Invalid module name index.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                const char* module_name = name_table_names[module_index];
                
                // Try to load as dynamic library
                ms_dynamic_extension_t* dyn_ext = try_load_library(module_name);
                if (dyn_ext) {
                    // Register the dynamically loaded extension
                    ms_register_extension(vm, dyn_ext->ext);
                    
                    // Store the dynamic extension for cleanup later
                    if (vm->dynamic_extension_count < 32) {
                        vm->dynamic_extensions[vm->dynamic_extension_count++] = dyn_ext;
                    }
                }
                
                // Create a module value
                ms_value_t module_val;
                module_val.type = MS_VAL_MODULE;
                module_val.as.module = (void*)module_name;  // Store module name as pointer
                
                ms_vm_push(vm, module_val);
                break;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_SHORT
#undef BINARY_OP
}

ms_vm_t* ms_vm_new(void) {
    ms_vm_t* vm = malloc(sizeof(ms_vm_t));
    ms_vm_reset_stack(vm);
    vm->globals = NULL;
    vm->has_error = false;
    vm->jit_enabled = false;
    vm->hotspot_threshold = 100;
    vm->frame_count = 0;
    vm->last_method_name = NULL;
    vm->last_module_name = NULL;
    vm->dynamic_extension_count = 0;
    return vm;
}

void ms_vm_free(ms_vm_t* vm) {
    // 卸载动态扩展
    for (int i = 0; i < vm->dynamic_extension_count; i++) {
        ms_unload_extension_library((ms_dynamic_extension_t*)vm->dynamic_extensions[i]);
    }
    
    // 释放全局变量
    ms_global_t* current = vm->globals;
    while (current != NULL) {
        ms_global_t* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    free(vm);
}

void ms_vm_reset_stack(ms_vm_t* vm) {
    vm->stack_top = vm->stack;
}

void ms_vm_push(ms_vm_t* vm, ms_value_t value) {
    *vm->stack_top = value;
    vm->stack_top++;
}

ms_value_t ms_vm_pop(ms_vm_t* vm) {
    vm->stack_top--;
    return *vm->stack_top;
}

ms_value_t ms_vm_peek(ms_vm_t* vm, int distance) {
    return vm->stack_top[-1 - distance];
}

ms_result_t ms_vm_interpret(ms_vm_t* vm, ms_chunk_t* chunk) {
    vm->chunk = chunk;
    vm->frames[0].ip = chunk->code;
    vm->frames[0].slots = vm->stack;
    vm->frame_count = 1;
    
    return run(vm);
}

const char* ms_vm_get_error(ms_vm_t* vm) {
    return vm->has_error ? vm->error_message : NULL;
}

void ms_vm_clear_error(ms_vm_t* vm) {
    vm->has_error = false;
}