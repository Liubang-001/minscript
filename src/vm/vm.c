#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
        if (!ms_value_is_int(peek(vm, 0)) || !ms_value_is_int(peek(vm, 1))) { \
            runtime_error(vm, "Operands must be numbers."); \
            return MS_RESULT_RUNTIME_ERROR; \
        } \
        int64_t b = ms_value_as_int(ms_vm_pop(vm)); \
        int64_t a = ms_value_as_int(ms_vm_pop(vm)); \
        ms_vm_push(vm, value_type(a op b)); \
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
                        printf(ms_value_as_bool(value) ? "true" : "false");
                        break;
                    case MS_VAL_NIL: printf("nil"); break;
                    case MS_VAL_INT: printf("%lld", ms_value_as_int(value)); break;
                    case MS_VAL_FLOAT: printf("%g", ms_value_as_float(value)); break;
                    case MS_VAL_STRING: printf("%s", ms_value_as_string(value)); break;
                    default: printf("<object>"); break;
                }
                printf("\n");
                break;
            }
            case OP_RETURN: {
                return MS_RESULT_OK;
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
    return vm;
}

void ms_vm_free(ms_vm_t* vm) {
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