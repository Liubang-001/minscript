#include "vm.h"
#include "../ext/ext.h"
#include "../core/class.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

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
#define READ_STRING() (name_table_names[READ_BYTE()])

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
                ms_value_t val = frame->slots[slot];
                ms_vm_push(vm, val);
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                frame->slots[slot] = peek(vm, 0);
                ms_vm_pop(vm);
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
                
                // 检查是否是实例对象，如果是则尝试调用 __eq__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__eq__")) {
                        ms_value_t eq_method = ms_dict_get(instance->klass->methods, "__eq__");
                        
                        if (eq_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = eq_method.as.function;
                            
                            // 调用 __eq__(self, other)
                            ms_vm_push(vm, a);
                            ms_vm_push(vm, b);
                            
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - 2;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - 2;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回值
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            break;
                        }
                    }
                }
                
                ms_vm_push(vm, ms_value_bool(values_equal(a, b)));
                break;
            }
            case OP_GREATER: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __gt__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__gt__")) {
                        ms_value_t gt_method = ms_dict_get(instance->klass->methods, "__gt__");
                        
                        if (gt_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = gt_method.as.function;
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_bool, >);
                break;
            }
            case OP_LESS: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __lt__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__lt__")) {
                        ms_value_t lt_method = ms_dict_get(instance->klass->methods, "__lt__");
                        
                        if (lt_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = lt_method.as.function;
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_bool, <);
                break;
            }
            case OP_LESS_EQUAL: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __le__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__le__")) {
                        ms_value_t le_method = ms_dict_get(instance->klass->methods, "__le__");
                        
                        if (le_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = le_method.as.function;
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_bool, <=);
                break;
            }
            case OP_GREATER_EQUAL: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __ge__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__ge__")) {
                        ms_value_t ge_method = ms_dict_get(instance->klass->methods, "__ge__");
                        
                        if (ge_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = ge_method.as.function;
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_bool, >=);
                break;
            }
            case OP_IN: {
                ms_value_t container = peek(vm, 0);  // Top of stack (right operand)
                ms_value_t item = peek(vm, 1);       // Below top (left operand)
                
                // 检查是否是实例对象，如果是则尝试调用 __contains__ 方法
                if (ms_value_is_instance(container)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(container);
                    
                    if (ms_dict_has(instance->klass->methods, "__contains__")) {
                        ms_value_t contains_method = ms_dict_get(instance->klass->methods, "__contains__");
                        
                        if (contains_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = contains_method.as.function;
                            
                            // 调用 __contains__(self, item)
                            // 栈上是 [item, container]，需要调整为 [container, item]
                            ms_vm_pop(vm);  // container
                            ms_vm_pop(vm);  // item
                            ms_vm_push(vm, container);  // self
                            ms_vm_push(vm, item);       // parameter
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - 2;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - 2;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                // 默认行为：检查列表、元组、字典、字符串
                // 先弹出操作数
                ms_vm_pop(vm);  // container
                ms_vm_pop(vm);  // item
                
                bool found = false;
                
                if (ms_value_is_list(container)) {
                    ms_list_t* list = ms_value_as_list(container);
                    for (int i = 0; i < ms_list_len(list); i++) {
                        ms_value_t elem = ms_list_get(list, i);
                        if (values_equal(item, elem)) {
                            found = true;
                            break;
                        }
                    }
                } else if (ms_value_is_tuple(container)) {
                    ms_tuple_t* tuple = ms_value_as_tuple(container);
                    for (int i = 0; i < ms_tuple_len(tuple); i++) {
                        ms_value_t elem = ms_tuple_get(tuple, i);
                        if (values_equal(item, elem)) {
                            found = true;
                            break;
                        }
                    }
                } else if (ms_value_is_dict(container)) {
                    if (ms_value_is_string(item)) {
                        ms_dict_t* dict = ms_value_as_dict(container);
                        found = ms_dict_has(dict, ms_value_as_string(item));
                    }
                } else if (ms_value_is_string(container) && ms_value_is_string(item)) {
                    const char* haystack = ms_value_as_string(container);
                    const char* needle = ms_value_as_string(item);
                    found = (strstr(haystack, needle) != NULL);
                } else {
                    runtime_error(vm, "Argument of type '%s' is not iterable.", "unknown");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_vm_push(vm, ms_value_bool(found));
                break;
            }
            case OP_ADD: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __add__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__add__")) {
                        ms_value_t add_method = ms_dict_get(instance->klass->methods, "__add__");
                        
                        if (add_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = add_method.as.function;
                            
                            // 调用 __add__(self, other)
                            // 栈上已经有 [a, b]，正好是我们需要的
                            
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - 2;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - 2;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回值
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            break;
                        }
                    }
                }
                
                // If either operand is a string, convert both to strings and concatenate
                if (ms_value_is_string(a) || ms_value_is_string(b)) {
                    ms_vm_pop(vm);
                    ms_vm_pop(vm);
                    
                    // Convert a to string
                    char a_str[256];
                    if (ms_value_is_string(a)) {
                        strcpy(a_str, ms_value_as_string(a));
                    } else if (ms_value_is_int(a)) {
                        snprintf(a_str, sizeof(a_str), "%lld", (long long)ms_value_as_int(a));
                    } else if (ms_value_is_float(a)) {
                        snprintf(a_str, sizeof(a_str), "%g", ms_value_as_float(a));
                    } else if (ms_value_is_bool(a)) {
                        strcpy(a_str, ms_value_as_bool(a) ? "True" : "False");
                    } else if (ms_value_is_nil(a)) {
                        strcpy(a_str, "None");
                    } else {
                        strcpy(a_str, "<object>");
                    }
                    
                    // Convert b to string
                    char b_str[256];
                    if (ms_value_is_string(b)) {
                        strcpy(b_str, ms_value_as_string(b));
                    } else if (ms_value_is_int(b)) {
                        snprintf(b_str, sizeof(b_str), "%lld", (long long)ms_value_as_int(b));
                    } else if (ms_value_is_float(b)) {
                        snprintf(b_str, sizeof(b_str), "%g", ms_value_as_float(b));
                    } else if (ms_value_is_bool(b)) {
                        strcpy(b_str, ms_value_as_bool(b) ? "True" : "False");
                    } else if (ms_value_is_nil(b)) {
                        strcpy(b_str, "None");
                    } else {
                        strcpy(b_str, "<object>");
                    }
                    
                    // Concatenate
                    char* result = malloc(strlen(a_str) + strlen(b_str) + 1);
                    strcpy(result, a_str);
                    strcat(result, b_str);
                    ms_vm_push(vm, ms_value_string(result));
                    free(result);
                } else if (ms_value_is_int(a) && ms_value_is_int(b)) {
                    ms_vm_pop(vm);
                    ms_vm_pop(vm);
                    ms_vm_push(vm, ms_value_int(ms_value_as_int(a) + ms_value_as_int(b)));
                } else {
                    runtime_error(vm, "Operands must be two numbers or two strings.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __sub__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__sub__")) {
                        ms_value_t sub_method = ms_dict_get(instance->klass->methods, "__sub__");
                        
                        if (sub_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = sub_method.as.function;
                            
                            // 调用 __sub__(self, other)
                            // 栈上已经有 [a, b]，正好是我们需要的
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回值
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            // 重新获取frame指针
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_int, -);
                break;
            }
            case OP_MULTIPLY: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __mul__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    if (ms_dict_has(instance->klass->methods, "__mul__")) {
                        ms_value_t mul_method = ms_dict_get(instance->klass->methods, "__mul__");
                        
                        if (mul_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = mul_method.as.function;
                            
                            // 调用 __mul__(self, other)
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回值
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            // 重新获取frame指针
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_int, *);
                break;
            }
            case OP_DIVIDE: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                
                // 检查是否是实例对象，如果是则尝试调用 __div__ 或 __truediv__ 方法
                if (ms_value_is_instance(a)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(a);
                    
                    // 优先尝试 __truediv__，然后是 __div__
                    const char* method_name = ms_dict_has(instance->klass->methods, "__truediv__") ? 
                                             "__truediv__" : "__div__";
                    
                    if (ms_dict_has(instance->klass->methods, method_name)) {
                        ms_value_t div_method = ms_dict_get(instance->klass->methods, method_name);
                        
                        if (div_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = div_method.as.function;
                            
                            // 调用 __div__(self, other) 或 __truediv__(self, other)
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回值
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            // 重新获取frame指针
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                BINARY_OP(ms_value_int, /);
                break;
            }
            case OP_FLOOR_DIVIDE: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                if (ms_value_is_int(a) && ms_value_is_int(b)) {
                    int64_t divisor = ms_value_as_int(b);
                    if (divisor == 0) {
                        runtime_error(vm, "Division by zero.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    ms_vm_pop(vm);
                    ms_vm_pop(vm);
                    ms_vm_push(vm, ms_value_int(ms_value_as_int(a) / divisor));
                } else {
                    double da = ms_value_as_float(a);
                    double db = ms_value_as_float(b);
                    if (db == 0.0) {
                        runtime_error(vm, "Division by zero.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    ms_vm_pop(vm);
                    ms_vm_pop(vm);
                    ms_vm_push(vm, ms_value_int((int64_t)(da / db)));
                }
                break;
            }
            case OP_POWER: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                double da = ms_value_as_float(a);
                double db = ms_value_as_float(b);
                ms_vm_pop(vm);
                ms_vm_pop(vm);
                double result = pow(da, db);
                // 如果结果是整数，返回整数类型
                if (result == (int64_t)result && result >= INT64_MIN && result <= INT64_MAX) {
                    ms_vm_push(vm, ms_value_int((int64_t)result));
                } else {
                    ms_vm_push(vm, ms_value_float(result));
                }
                break;
            }
            case OP_MODULO: {
                ms_value_t b = peek(vm, 0);
                ms_value_t a = peek(vm, 1);
                if (ms_value_is_int(a) && ms_value_is_int(b)) {
                    int64_t divisor = ms_value_as_int(b);
                    if (divisor == 0) {
                        runtime_error(vm, "Modulo by zero.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    ms_vm_pop(vm);
                    ms_vm_pop(vm);
                    ms_vm_push(vm, ms_value_int(ms_value_as_int(a) % divisor));
                } else {
                    runtime_error(vm, "Modulo operands must be integers.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
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
                
                // 检查是否是实例对象，如果是则尝试调用 __str__ 方法
                if (ms_value_is_instance(value)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(value);
                    
                    // 查找 __str__ 方法
                    if (ms_dict_has(instance->klass->methods, "__str__")) {
                        ms_value_t str_method = ms_dict_get(instance->klass->methods, "__str__");
                        
                        if (str_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = str_method.as.function;
                            
                            // 调用 __str__(self)
                            ms_vm_push(vm, value);  // self
                            
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - 1;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - 1;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回的字符串
                            ms_value_t str_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            
                            if (ms_value_is_string(str_value)) {
                                printf("%s\n", ms_value_as_string(str_value));
                            } else {
                                printf("<object>\n");
                            }
                            break;
                        }
                    }
                }
                
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
                
                // 调试输出
                // fprintf(stderr, "DEBUG OP_CALL: arg_count=%d, func_val.type=%d\n", arg_count, func_val.type);
                
                // 检查是否是绑定方法调用
                if (ms_value_is_bound_method(func_val)) {
                    ms_bound_method_t* bound = (ms_bound_method_t*)ms_value_as_bound_method(func_val);
                    ms_value_t method = bound->method;
                    ms_value_t receiver = bound->receiver;
                    
                    // 将 receiver (self) 插入到参数列表的开头
                    // 栈布局: [bound_method, arg1, arg2, ...] -> [receiver, arg1, arg2, ...]
                    vm->stack_top[-arg_count - 1] = receiver;
                    
                    // 调用方法
                    if (method.type == MS_VAL_FUNCTION) {
                        ms_function_t* function = method.as.function;
                        
                        // 检查参数数量（包括 self）
                        int min_args = function->arity - function->default_count;
                        int max_args = function->arity;
                        int total_args = arg_count + 1;  // +1 for self
                        
                        if (total_args < min_args || total_args > max_args) {
                            runtime_error(vm, "Expected %d to %d arguments but got %d.", 
                                        min_args, max_args, total_args);
                            return MS_RESULT_RUNTIME_ERROR;
                        }
                        
                        // 填充默认参数
                        int missing_args = function->arity - total_args;
                        if (missing_args > 0) {
                            int default_start = function->default_count - missing_args;
                            for (int i = 0; i < missing_args; i++) {
                                ms_vm_push(vm, function->defaults[default_start + i]);
                            }
                        }
                        
                        // 创建调用帧
                        if (vm->frame_count >= 64) {
                            runtime_error(vm, "Stack overflow.");
                            return MS_RESULT_RUNTIME_ERROR;
                        }
                        
                        // 注意：栈上现在是 [receiver, arg1, arg2, ...]，没有bound_method
                        // 所以call_stack_base应该指向receiver之前的位置
                        ms_value_t* call_stack_base = vm->stack_top - function->arity;
                        ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                        new_frame->ip = function->chunk->code;
                        new_frame->slots = vm->stack_top - function->arity;
                        
                        ms_chunk_t* prev_chunk = vm->chunk;
                        vm->chunk = function->chunk;
                        
                        ms_result_t result = run(vm);
                        
                        vm->chunk = prev_chunk;
                        vm->frame_count--;
                        
                        if (result != MS_RESULT_OK) {
                            return result;
                        }
                        
                        // 函数返回值应该在栈上
                        ms_value_t return_value = ms_vm_pop(vm);
                        vm->stack_top = call_stack_base;
                        ms_vm_push(vm, return_value);
                        
                        // 重新获取当前frame指针（递归调用后可能失效）
                        frame = &vm->frames[vm->frame_count - 1];
                    }
                    break;
                }
                
                // 检查是否是类实例化
                if (ms_value_is_class(func_val)) {
                    ms_class_t* klass = (ms_class_t*)ms_value_as_class(func_val);
                    
                    // 创建实例
                    ms_instance_t* instance = ms_instance_new(klass);
                    ms_value_t instance_val = ms_value_instance(instance);
                    
                    // 查找 __init__ 方法
                    if (ms_dict_has(klass->methods, "__init__")) {
                        ms_value_t init_method = ms_dict_get(klass->methods, "__init__");
                        
                        // 将实例作为第一个参数（self）
                        // 栈布局: [class, arg1, arg2, ...] -> [instance, arg1, arg2, ...]
                        vm->stack_top[-arg_count - 1] = instance_val;
                        
                        // 调用 __init__
                        if (init_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = init_method.as.function;
                            
                            // 检查参数数量（包括 self）
                            int min_args = function->arity - function->default_count;
                            int max_args = function->arity;
                            int total_args = arg_count + 1;  // +1 for self
                            
                            if (total_args < min_args || total_args > max_args) {
                                runtime_error(vm, "__init__() takes %d to %d arguments but %d were given.", 
                                            min_args, max_args, total_args);
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            // 填充默认参数
                            int missing_args = function->arity - total_args;
                            if (missing_args > 0) {
                                int default_start = function->default_count - missing_args;
                                for (int i = 0; i < missing_args; i++) {
                                    ms_vm_push(vm, function->defaults[default_start + i]);
                                }
                            }
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            // 注意：栈上现在是 [instance, arg1, arg2, ...]，没有class
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // __init__ 返回 None，我们返回实例
                            ms_vm_pop(vm);  // 弹出 __init__ 的返回值
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, instance_val);
                            
                            // 重新获取当前frame指针（递归调用后可能失效）
                            frame = &vm->frames[vm->frame_count - 1];
                        }
                    } else {
                        // 没有 __init__，直接返回实例
                        vm->stack_top -= arg_count + 1;
                        ms_vm_push(vm, instance_val);
                    }
                    break;
                }
                
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
                    ms_value_t* stack_base = vm->stack_top - arg_count - 1;  // 保存栈基址
                    ms_value_t result = func_val.as.native_func->func(vm, arg_count, args);
                    vm->stack_top = stack_base;  // 恢复到函数调用前
                    ms_vm_push(vm, result);
                } else if (func_val.type == MS_VAL_FUNCTION) {
                    // 用户定义的函数调用
                    ms_function_t* function = func_val.as.function;
                    
                    // 检查参数数量（考虑默认参数）
                    int min_args = function->arity - function->default_count;
                    int max_args = function->arity;
                    
                    if (arg_count < min_args || arg_count > max_args) {
                        if (function->default_count > 0) {
                            runtime_error(vm, "Expected %d to %d arguments but got %d.", 
                                        min_args, max_args, arg_count);
                        } else {
                            runtime_error(vm, "Expected %d arguments but got %d.", 
                                        function->arity, arg_count);
                        }
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    
                    if (vm->frame_count >= 64) {
                        runtime_error(vm, "Stack overflow.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    
                    // 填充缺失的默认参数
                    int missing_args = function->arity - arg_count;
                    if (missing_args > 0) {
                        // 从defaults数组的末尾开始填充
                        int default_start = function->default_count - missing_args;
                        for (int i = 0; i < missing_args; i++) {
                            ms_vm_push(vm, function->defaults[default_start + i]);
                        }
                    }
                    
                    // 保存栈指针位置（函数和参数之前）
                    ms_value_t* call_stack_base = vm->stack_top - function->arity - 1;
                    
                    // 创建新的调用帧
                    ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                    new_frame->ip = function->chunk->code;
                    new_frame->slots = vm->stack_top - function->arity;
                    
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
                    
                    // 重新获取当前frame指针（递归调用后可能失效）
                    frame = &vm->frames[vm->frame_count - 1];
                } else {
                    runtime_error(vm, "Can only call functions.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_CALL_DECORATOR: {
                // Apply decorator to function/class
                // Stack: [dec1, dec2, ..., decN, target]
                // depth indicates which decorator to apply (1 = decN, 2 = dec(N-1), etc.)
                uint8_t depth = READ_BYTE();
                
                // Get the target (function or class) from top of stack
                ms_value_t target = ms_vm_pop(vm);
                
                // Get the decorator at position depth from top
                ms_value_t decorator = *(vm->stack_top - depth);
                
                // Remove the decorator from its position
                for (int i = depth; i > 1; i--) {
                    *(vm->stack_top - i) = *(vm->stack_top - i + 1);
                }
                vm->stack_top--;
                
                // Now call decorator(target)
                ms_vm_push(vm, decorator);
                ms_vm_push(vm, target);
                
                // Check if decorator is callable
                if (decorator.type != MS_VAL_FUNCTION && decorator.type != MS_VAL_NATIVE_FUNC) {
                    runtime_error(vm, "Decorator must be callable.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Call the decorator with 1 argument
                if (decorator.type == MS_VAL_NATIVE_FUNC) {
                    ms_value_t* args = vm->stack_top - 1;
                    ms_value_t* stack_base = vm->stack_top - 2;
                    ms_value_t result = decorator.as.native_func->func(vm, 1, args);
                    vm->stack_top = stack_base;
                    ms_vm_push(vm, result);
                } else if (decorator.type == MS_VAL_FUNCTION) {
                    ms_function_t* function = decorator.as.function;
                    
                    if (function->arity != 1) {
                        runtime_error(vm, "Decorator must take exactly 1 argument.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    
                    if (vm->frame_count >= 64) {
                        runtime_error(vm, "Stack overflow.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    
                    ms_value_t* call_stack_base = vm->stack_top - 2;
                    ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                    new_frame->ip = function->chunk->code;
                    new_frame->slots = vm->stack_top - 1;
                    
                    ms_chunk_t* prev_chunk = vm->chunk;
                    vm->chunk = function->chunk;
                    
                    ms_result_t result = run(vm);
                    
                    vm->chunk = prev_chunk;
                    vm->frame_count--;
                    
                    if (result != MS_RESULT_OK) {
                        return result;
                    }
                    
                    ms_value_t return_value = ms_vm_pop(vm);
                    vm->stack_top = call_stack_base;
                    ms_vm_push(vm, return_value);
                    
                    frame = &vm->frames[vm->frame_count - 1];
                }
                
                break;
            }
            case OP_CALL_ENTER: {
                // Call __enter__() method on context manager
                // Stack: [manager, manager] (duplicated by parser)
                ms_value_t manager = peek(vm, 0);
                
                // Check if manager is an instance
                if (!ms_value_is_instance(manager)) {
                    runtime_error(vm, "Context manager must be an instance.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_instance_t* instance = ms_value_as_instance(manager);
                
                // Look up __enter__ method
                ms_value_t enter_method = ms_value_nil();
                bool found = false;
                
                if (instance->klass && instance->klass->methods) {
                    for (int i = 0; i < instance->klass->methods->count; i++) {
                        if (instance->klass->methods->entries[i].key != NULL &&
                            strcmp(instance->klass->methods->entries[i].key, "__enter__") == 0) {
                            enter_method = instance->klass->methods->entries[i].value;
                            found = true;
                            break;
                        }
                    }
                }
                
                if (!found) {
                    runtime_error(vm, "Context manager has no __enter__ method.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Check if __enter__ is a function
                if (enter_method.type != MS_VAL_FUNCTION) {
                    runtime_error(vm, "__enter__ must be a method.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_function_t* function = enter_method.as.function;
                
                // Create call frame for __enter__
                if (vm->frame_count >= 64) {
                    runtime_error(vm, "Stack overflow.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Stack: [manager, manager] -> push self for method call
                ms_vm_push(vm, manager);
                
                ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                new_frame->ip = function->chunk->code;
                new_frame->slots = vm->stack_top - 1;
                
                ms_chunk_t* prev_chunk = vm->chunk;
                vm->chunk = function->chunk;
                
                ms_result_t result = run(vm);
                
                vm->chunk = prev_chunk;
                vm->frame_count--;
                
                if (result != MS_RESULT_OK) {
                    return result;
                }
                
                // Get return value from __enter__
                ms_value_t return_value = ms_vm_pop(vm);
                
                // Stack is now [manager, manager] - pop the duplicate, keep original
                ms_vm_pop(vm);  // Pop the duplicate we used for the call
                
                // Stack is now [manager] - push the return value
                ms_vm_push(vm, return_value);
                
                // Stack is now [manager, return_value]
                // The parser will handle storing return_value or popping it
                
                // Refresh frame pointer
                frame = &vm->frames[vm->frame_count - 1];
                
                break;
            }
            case OP_CALL_EXIT: {
                // Call __exit__(None, None, None) method on context manager
                // Stack: [manager]
                ms_value_t manager = ms_vm_pop(vm);
                
                // Check if manager is an instance
                if (!ms_value_is_instance(manager)) {
                    runtime_error(vm, "Context manager must be an instance.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_instance_t* instance = ms_value_as_instance(manager);
                
                // Look up __exit__ method
                ms_value_t exit_method = ms_value_nil();
                bool found = false;
                
                if (instance->klass && instance->klass->methods) {
                    for (int i = 0; i < instance->klass->methods->count; i++) {
                        if (instance->klass->methods->entries[i].key != NULL &&
                            strcmp(instance->klass->methods->entries[i].key, "__exit__") == 0) {
                            exit_method = instance->klass->methods->entries[i].value;
                            found = true;
                            break;
                        }
                    }
                }
                
                if (!found) {
                    runtime_error(vm, "Context manager has no __exit__ method.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Check if __exit__ is a function
                if (exit_method.type != MS_VAL_FUNCTION) {
                    runtime_error(vm, "__exit__ must be a method.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_function_t* function = exit_method.as.function;
                
                // Create call frame for __exit__
                if (vm->frame_count >= 64) {
                    runtime_error(vm, "Stack overflow.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Stack setup: [self, None, None, None] for __exit__(exc_type, exc_val, exc_tb)
                ms_vm_push(vm, manager);
                ms_vm_push(vm, ms_value_nil());
                ms_vm_push(vm, ms_value_nil());
                ms_vm_push(vm, ms_value_nil());
                
                ms_value_t* call_stack_base = vm->stack_top - 4;
                ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                new_frame->ip = function->chunk->code;
                new_frame->slots = vm->stack_top - 4;
                
                ms_chunk_t* prev_chunk = vm->chunk;
                vm->chunk = function->chunk;
                
                ms_result_t result = run(vm);
                
                vm->chunk = prev_chunk;
                vm->frame_count--;
                
                if (result != MS_RESULT_OK) {
                    return result;
                }
                
                // Pop return value (we don't use it for now)
                ms_vm_pop(vm);
                
                // Refresh frame pointer
                frame = &vm->frames[vm->frame_count - 1];
                
                break;
            }
            case OP_ASSERT: {
                // assert 语句: 栈上有 [condition, message]
                ms_value_t message = ms_vm_pop(vm);
                ms_value_t condition = ms_vm_pop(vm);
                
                // 检查条件是否为真
                if (!ms_value_as_bool(condition)) {
                    // 断言失败
                    if (ms_value_is_string(message)) {
                        runtime_error(vm, "AssertionError: %s", ms_value_as_string(message));
                    } else {
                        runtime_error(vm, "AssertionError");
                    }
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_DELETE: {
                // del 语句: 删除全局变量
                uint8_t name_index = READ_BYTE();
                if (name_index >= name_table_count) {
                    runtime_error(vm, "Invalid variable name index.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                char* name = name_table_names[name_index];
                
                // 查找并删除全局变量
                ms_global_t* prev = NULL;
                ms_global_t* current = vm->globals;
                while (current != NULL) {
                    if (strcmp(current->name, name) == 0) {
                        // 找到了，删除它
                        if (prev == NULL) {
                            vm->globals = current->next;
                        } else {
                            prev->next = current->next;
                        }
                        free(current->name);
                        free(current);
                        goto deleted_global;
                    }
                    prev = current;
                    current = current->next;
                }
                
                runtime_error(vm, "Undefined variable '%s'.", name);
                return MS_RESULT_RUNTIME_ERROR;
                
            deleted_global:
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
                
                // 清除模块方法状态
                vm->last_method_name = NULL;
                vm->last_module_name = NULL;
                
                // 检查是否是实例对象
                if (ms_value_is_instance(obj)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(obj);
                    
                    // 先查找实例属性
                    if (ms_dict_has(instance->attrs, prop_name)) {
                        ms_value_t value = ms_dict_get(instance->attrs, prop_name);
                        ms_vm_push(vm, value);
                        break;
                    }
                    
                    // 查找方法
                    if (ms_dict_has(instance->klass->methods, prop_name)) {
                        ms_value_t method = ms_dict_get(instance->klass->methods, prop_name);
                        // 创建绑定方法
                        ms_bound_method_t* bound = ms_bound_method_new(obj, method);
                        
                        // 创建绑定方法值
                        ms_value_t bound_val = ms_value_bound_method(bound);
                        ms_vm_push(vm, bound_val);
                        break;
                    }
                    
                    runtime_error(vm, "Undefined property '%s'.", prop_name);
                    return MS_RESULT_RUNTIME_ERROR;
                }
                // If it's a module, store the method name for the call handler
                else if (obj.type == MS_VAL_MODULE) {
                    vm->last_module_name = (const char*)obj.as.module;
                    vm->last_method_name = prop_name;
                    ms_vm_push(vm, obj);
                } else {
                    // For other types, just return nil
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
            case OP_BUILD_LIST: {
                uint8_t count = READ_BYTE();
                ms_list_t* list = ms_list_new();
                for (int i = 0; i < count; i++) {
                    ms_list_append(list, vm->stack_top[-count + i]);
                }
                vm->stack_top -= count;
                ms_vm_push(vm, ms_value_list(list));
                break;
            }
            case OP_BUILD_DICT: {
                uint8_t pair_count = READ_BYTE();
                ms_dict_t* dict = ms_dict_new();
                for (int i = 0; i < pair_count; i++) {
                    ms_value_t value = ms_vm_pop(vm);
                    ms_value_t key_val = ms_vm_pop(vm);
                    if (!ms_value_is_string(key_val)) {
                        runtime_error(vm, "Dictionary keys must be strings.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    ms_dict_set(dict, ms_value_as_string(key_val), value);
                }
                ms_vm_push(vm, ms_value_dict(dict));
                break;
            }
            case OP_BUILD_TUPLE: {
                uint8_t count = READ_BYTE();
                ms_tuple_t* tuple = ms_tuple_new(count);
                for (int i = 0; i < count; i++) {
                    tuple->elements[i] = vm->stack_top[-count + i];
                }
                vm->stack_top -= count;
                ms_vm_push(vm, ms_value_tuple(tuple));
                break;
            }
            case OP_BUILD_SET: {
                uint8_t count = READ_BYTE();
                ms_set_t* set = ms_set_new();
                for (int i = 0; i < count; i++) {
                    ms_set_add(set, vm->stack_top[-count + i]);
                }
                vm->stack_top -= count;
                ms_vm_push(vm, ms_value_set(set));
                break;
            }
            case OP_SET_ADD: {
                // Stack: [set, element]
                ms_value_t element = ms_vm_pop(vm);
                ms_value_t set_val = ms_vm_pop(vm);
                
                if (!ms_value_is_set(set_val)) {
                    runtime_error(vm, "Can only add to sets.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_set_t* set = ms_value_as_set(set_val);
                ms_set_add(set, element);
                
                // Push set back
                ms_vm_push(vm, set_val);
                break;
            }
            case OP_INDEX_GET: {
                ms_value_t index_val = ms_vm_pop(vm);
                ms_value_t obj = ms_vm_pop(vm);
                
                // 检查是否是实例对象，如果是则尝试调用 __getitem__ 方法
                if (ms_value_is_instance(obj)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(obj);
                    
                    if (ms_dict_has(instance->klass->methods, "__getitem__")) {
                        ms_value_t getitem_method = ms_dict_get(instance->klass->methods, "__getitem__");
                        
                        if (getitem_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = getitem_method.as.function;
                            
                            // 调用 __getitem__(self, key)
                            ms_vm_push(vm, obj);       // self
                            ms_vm_push(vm, index_val); // key
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 获取返回值
                            ms_value_t return_value = ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            ms_vm_push(vm, return_value);
                            
                            // 重新获取frame指针
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                if (ms_value_is_list(obj)) {
                    if (!ms_value_is_int(index_val)) {
                        runtime_error(vm, "List indices must be integers.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    int index = (int)ms_value_as_int(index_val);
                    ms_list_t* list = ms_value_as_list(obj);
                    ms_vm_push(vm, ms_list_get(list, index));
                } else if (ms_value_is_dict(obj)) {
                    if (!ms_value_is_string(index_val)) {
                        runtime_error(vm, "Dictionary keys must be strings.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    ms_dict_t* dict = ms_value_as_dict(obj);
                    ms_vm_push(vm, ms_dict_get(dict, ms_value_as_string(index_val)));
                } else if (ms_value_is_tuple(obj)) {
                    if (!ms_value_is_int(index_val)) {
                        runtime_error(vm, "Tuple indices must be integers.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    int index = (int)ms_value_as_int(index_val);
                    ms_tuple_t* tuple = ms_value_as_tuple(obj);
                    ms_vm_push(vm, ms_tuple_get(tuple, index));
                } else {
                    runtime_error(vm, "Can only index lists, dicts, and tuples.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_INDEX_SET: {
                ms_value_t value = ms_vm_pop(vm);
                ms_value_t index_val = ms_vm_pop(vm);
                ms_value_t obj = ms_vm_pop(vm);
                
                // 检查是否是实例对象，如果是则尝试调用 __setitem__ 方法
                if (ms_value_is_instance(obj)) {
                    ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(obj);
                    
                    if (ms_dict_has(instance->klass->methods, "__setitem__")) {
                        ms_value_t setitem_method = ms_dict_get(instance->klass->methods, "__setitem__");
                        
                        if (setitem_method.type == MS_VAL_FUNCTION) {
                            ms_function_t* function = setitem_method.as.function;
                            
                            // 调用 __setitem__(self, key, value)
                            ms_vm_push(vm, obj);       // self
                            ms_vm_push(vm, index_val); // key
                            ms_vm_push(vm, value);     // value
                            
                            // 创建调用帧
                            if (vm->frame_count >= 64) {
                                runtime_error(vm, "Stack overflow.");
                                return MS_RESULT_RUNTIME_ERROR;
                            }
                            
                            ms_value_t* call_stack_base = vm->stack_top - function->arity;
                            ms_call_frame_t* new_frame = &vm->frames[vm->frame_count++];
                            new_frame->ip = function->chunk->code;
                            new_frame->slots = vm->stack_top - function->arity;
                            
                            ms_chunk_t* prev_chunk = vm->chunk;
                            vm->chunk = function->chunk;
                            
                            ms_result_t result = run(vm);
                            
                            vm->chunk = prev_chunk;
                            vm->frame_count--;
                            
                            if (result != MS_RESULT_OK) {
                                return result;
                            }
                            
                            // 弹出返回值（__setitem__通常返回None）
                            ms_vm_pop(vm);
                            vm->stack_top = call_stack_base;
                            
                            // 重新获取frame指针
                            frame = &vm->frames[vm->frame_count - 1];
                            break;
                        }
                    }
                }
                
                if (ms_value_is_list(obj)) {
                    if (!ms_value_is_int(index_val)) {
                        runtime_error(vm, "List indices must be integers.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    int index = (int)ms_value_as_int(index_val);
                    ms_list_t* list = ms_value_as_list(obj);
                    ms_list_set(list, index, value);
                } else if (ms_value_is_dict(obj)) {
                    if (!ms_value_is_string(index_val)) {
                        runtime_error(vm, "Dictionary keys must be strings.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    ms_dict_t* dict = ms_value_as_dict(obj);
                    ms_dict_set(dict, ms_value_as_string(index_val), value);
                } else {
                    runtime_error(vm, "Can only index lists and dicts.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SLICE_GET: {
                // Stack: obj, start, stop, step
                ms_value_t step_val = ms_vm_pop(vm);
                ms_value_t stop_val = ms_vm_pop(vm);
                ms_value_t start_val = ms_vm_pop(vm);
                ms_value_t obj = ms_vm_pop(vm);
                
                // Default values
                int start = 0;
                int stop = 0;
                int step = 1;
                
                // Parse step
                if (!ms_value_is_nil(step_val)) {
                    if (!ms_value_is_int(step_val)) {
                        runtime_error(vm, "Slice step must be an integer.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    step = (int)ms_value_as_int(step_val);
                    if (step == 0) {
                        runtime_error(vm, "Slice step cannot be zero.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                }
                
                // Get object length
                int len = 0;
                if (ms_value_is_list(obj)) {
                    len = ms_list_len(ms_value_as_list(obj));
                } else if (ms_value_is_tuple(obj)) {
                    len = ms_tuple_len(ms_value_as_tuple(obj));
                } else if (ms_value_is_string(obj)) {
                    len = strlen(ms_value_as_string(obj));
                } else {
                    runtime_error(vm, "Can only slice lists, tuples, and strings.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Parse start
                if (ms_value_is_nil(start_val)) {
                    start = (step > 0) ? 0 : len - 1;
                } else {
                    if (!ms_value_is_int(start_val)) {
                        runtime_error(vm, "Slice start must be an integer.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    start = (int)ms_value_as_int(start_val);
                }
                
                // Parse stop
                if (ms_value_is_nil(stop_val)) {
                    stop = (step > 0) ? len : -len - 1;
                } else {
                    if (!ms_value_is_int(stop_val)) {
                        runtime_error(vm, "Slice stop must be an integer.");
                        return MS_RESULT_RUNTIME_ERROR;
                    }
                    stop = (int)ms_value_as_int(stop_val);
                }
                
                // Perform slice
                ms_value_t result;
                if (ms_value_is_list(obj)) {
                    result = ms_slice_list(ms_value_as_list(obj), start, stop, step);
                } else if (ms_value_is_tuple(obj)) {
                    result = ms_slice_tuple(ms_value_as_tuple(obj), start, stop, step);
                } else if (ms_value_is_string(obj)) {
                    result = ms_slice_string(ms_value_as_string(obj), start, stop, step);
                } else {
                    result = ms_value_nil();
                }
                
                ms_vm_push(vm, result);
                break;
            }
            case OP_FOR_ITER: {
                uint8_t var_slot = READ_BYTE();
                
                // Stack should have: [..., iterable, index]
                // Peek at the values without popping
                ms_value_t index_val = peek(vm, 0);
                ms_value_t iterable = peek(vm, 1);
                
                if (!ms_value_is_int(index_val)) {
                    runtime_error(vm, "For loop index must be an integer.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                int index = (int)ms_value_as_int(index_val);
                ms_value_t current_element = ms_value_nil();
                bool has_next = false;
                
                if (ms_value_is_list(iterable)) {
                    ms_list_t* list = ms_value_as_list(iterable);
                    if (index < ms_list_len(list)) {
                        current_element = ms_list_get(list, index);
                        has_next = true;
                    }
                } else if (ms_value_is_dict(iterable)) {
                    ms_dict_t* dict = ms_value_as_dict(iterable);
                    if (index < ms_dict_len(dict)) {
                        // For dicts, iterate over keys
                        current_element = ms_value_string(dict->entries[index].key);
                        has_next = true;
                    }
                } else if (ms_value_is_tuple(iterable)) {
                    ms_tuple_t* tuple = ms_value_as_tuple(iterable);
                    if (index < ms_tuple_len(tuple)) {
                        current_element = ms_tuple_get(tuple, index);
                        has_next = true;
                    }
                } else if (ms_value_is_string(iterable)) {
                    // Support string iteration
                    const char* str = ms_value_as_string(iterable);
                    int str_len = strlen(str);
                    if (index < str_len) {
                        char char_str[2] = {str[index], '\0'};
                        current_element = ms_value_string(char_str);
                        has_next = true;
                    }
                } else {
                    runtime_error(vm, "Can only iterate over lists, dicts, tuples, and strings.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Set the loop variable to current element
                if (var_slot < MS_MAX_LOCALS) {
                    frame->slots[var_slot] = current_element;
                }
                
                // Update the index on the stack (increment it for next iteration)
                vm->stack_top[-1] = ms_value_int(index + 1);
                
                // Push whether we have more elements (for jump condition)
                ms_vm_push(vm, ms_value_bool(has_next));
                break;
            }
            case OP_FOR_ITER_LOCAL: {
                uint8_t var_slot = READ_BYTE();
                uint8_t iter_slot = READ_BYTE();
                uint8_t index_slot = READ_BYTE();
                
                // Get iterable and index from local variables
                ms_value_t iterable = frame->slots[iter_slot];
                ms_value_t index_val = frame->slots[index_slot];
                
                if (!ms_value_is_int(index_val)) {
                    runtime_error(vm, "For loop index must be an integer.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                int index = (int)ms_value_as_int(index_val);
                ms_value_t current_element = ms_value_nil();
                bool has_next = false;
                
                if (ms_value_is_list(iterable)) {
                    ms_list_t* list = ms_value_as_list(iterable);
                    if (index < ms_list_len(list)) {
                        current_element = ms_list_get(list, index);
                        has_next = true;
                    }
                } else if (ms_value_is_dict(iterable)) {
                    ms_dict_t* dict = ms_value_as_dict(iterable);
                    if (index < ms_dict_len(dict)) {
                        // For dicts, iterate over keys
                        current_element = ms_value_string(dict->entries[index].key);
                        has_next = true;
                    }
                } else if (ms_value_is_tuple(iterable)) {
                    ms_tuple_t* tuple = ms_value_as_tuple(iterable);
                    if (index < ms_tuple_len(tuple)) {
                        current_element = ms_tuple_get(tuple, index);
                        has_next = true;
                    }
                } else if (ms_value_is_string(iterable)) {
                    // Support string iteration
                    const char* str = ms_value_as_string(iterable);
                    int str_len = strlen(str);
                    if (index < str_len) {
                        char char_str[2] = {str[index], '\0'};
                        current_element = ms_value_string(char_str);
                        has_next = true;
                    }
                } else {
                    runtime_error(vm, "Can only iterate over lists, dicts, tuples, and strings.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                // Set the loop variable to current element
                if (var_slot < MS_MAX_LOCALS) {
                    frame->slots[var_slot] = current_element;
                }
                
                // Update the index in local variable (increment it for next iteration)
                frame->slots[index_slot] = ms_value_int(index + 1);
                
                // Push whether we have more elements (for jump condition)
                ms_vm_push(vm, ms_value_bool(has_next));
                break;
            }
            case OP_TERNARY: {
                // 栈顶: [value_if_false, condition, value_if_true]
                // 弹出三个值，根据条件返回相应的值
                ms_value_t value_if_false = ms_vm_pop(vm);
                ms_value_t condition = ms_vm_pop(vm);
                ms_value_t value_if_true = ms_vm_pop(vm);
                
                // 判断条件是否为真
                bool is_true = !is_falsey(condition);
                
                // 推送相应的值
                ms_vm_push(vm, is_true ? value_if_true : value_if_false);
                break;
            }
            case OP_DUP: {
                // 复制栈顶值
                ms_value_t top = *(vm->stack_top - 1);
                ms_vm_push(vm, top);
                break;
            }
            case OP_SWAP: {
                // 交换栈顶两个值
                if (vm->stack_top - vm->stack < 2) {
                    runtime_error(vm, "Stack underflow in swap.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                ms_value_t top = *(vm->stack_top - 1);
                ms_value_t second = *(vm->stack_top - 2);
                *(vm->stack_top - 1) = second;
                *(vm->stack_top - 2) = top;
                break;
            }
            case OP_BUILD_LIST_COMP: {
                // 列表推导式: [expr for var in iterable] 或 [expr if cond else expr2 for var in iterable]
                // 栈顶: [iterable, condition (if has_condition), expr]
                uint8_t var_index = READ_BYTE();
                uint8_t has_condition = READ_BYTE();
                
                ms_value_t iterable = ms_vm_pop(vm);
                ms_value_t condition_val = ms_value_nil();
                ms_value_t expr_val = ms_value_nil();
                
                if (has_condition) {
                    condition_val = ms_vm_pop(vm);
                }
                expr_val = ms_vm_pop(vm);
                
                // 创建新列表
                ms_list_t* result_list = ms_list_new();
                
                // 遍历可迭代对象
                if (ms_value_is_list(iterable)) {
                    ms_list_t* list = ms_value_as_list(iterable);
                    for (int i = 0; i < list->count; i++) {
                        ms_value_t item = list->elements[i];
                        
                        // 如果有条件，检查条件
                        if (has_condition) {
                            // 这里需要重新评估条件和表达式
                            // 但由于我们已经弹出了值，我们需要另一种方法
                            // 实际上，列表推导式需要在编译时生成循环代码
                            // 而不是在运行时处理
                        }
                        
                        ms_list_append(result_list, item);
                    }
                } else if (ms_value_is_string(iterable)) {
                    const char* str = ms_value_as_string(iterable);
                    for (int i = 0; str[i] != '\0'; i++) {
                        char char_str[2] = {str[i], '\0'};
                        ms_list_append(result_list, ms_value_string(char_str));
                    }
                } else {
                    runtime_error(vm, "Can only iterate over lists and strings in list comprehension.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_vm_push(vm, ms_value_list(result_list));
                break;
            }
            case OP_LIST_APPEND: {
                // 向列表添加元素
                // 栈顶: [list, element]
                ms_value_t element = ms_vm_pop(vm);
                ms_value_t list_val = ms_vm_pop(vm);
                
                if (!ms_value_is_list(list_val)) {
                    runtime_error(vm, "Can only append to lists.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_list_t* list = ms_value_as_list(list_val);
                ms_list_append(list, element);
                
                // 推送列表回栈
                ms_vm_push(vm, list_val);
                break;
            }
            case OP_CLASS: {
                // 创建类对象
                const char* name = READ_STRING();
                ms_class_t* klass = ms_class_new(name);
                ms_vm_push(vm, ms_value_class(klass));
                break;
            }
            case OP_INHERIT: {
                // 设置继承关系
                // 栈顶: [superclass, subclass]
                ms_value_t superclass = peek(vm, 1);
                ms_value_t subclass = peek(vm, 0);
                
                if (!ms_value_is_class(superclass)) {
                    runtime_error(vm, "Superclass must be a class.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                if (!ms_value_is_class(subclass)) {
                    runtime_error(vm, "Subclass must be a class.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_class_t* super = (ms_class_t*)ms_value_as_class(superclass);
                ms_class_t* sub = (ms_class_t*)ms_value_as_class(subclass);
                
                // 设置父类
                sub->parent = super;
                
                // 继承父类的方法（浅拷贝）
                if (super->methods) {
                    for (int i = 0; i < super->methods->capacity; i++) {
                        if (super->methods->entries[i].key != NULL) {
                            ms_dict_set(sub->methods, super->methods->entries[i].key, 
                                       super->methods->entries[i].value);
                        }
                    }
                }
                
                ms_vm_pop(vm);  // 弹出子类
                break;
            }
            case OP_METHOD: {
                // 添加方法到类
                // 栈顶: [class, method]
                const char* name = READ_STRING();
                ms_value_t method = peek(vm, 0);
                ms_value_t class_val = peek(vm, 1);
                
                if (!ms_value_is_class(class_val)) {
                    runtime_error(vm, "Can only add methods to classes.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                ms_class_t* klass = (ms_class_t*)ms_value_as_class(class_val);
                ms_dict_set(klass->methods, name, method);
                
                ms_vm_pop(vm);  // 弹出方法
                break;
            }
            case OP_SET_PROPERTY: {
                // 设置属性
                // 栈顶: [instance, value]
                if (!ms_value_is_instance(peek(vm, 1))) {
                    runtime_error(vm, "Only instances have properties.");
                    return MS_RESULT_RUNTIME_ERROR;
                }
                
                uint8_t name_index = READ_BYTE();
                const char* name = name_table_names[name_index];
                ms_instance_t* instance = (ms_instance_t*)ms_value_as_instance(peek(vm, 1));
                ms_value_t value = peek(vm, 0);
                
                ms_dict_set(instance->attrs, name, value);
                
                ms_vm_pop(vm);  // 弹出值
                ms_vm_pop(vm);  // 弹出实例
                ms_vm_push(vm, value);  // 推送值回栈
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