#include "miniscript.h"
#include "../vm/vm.h"
#include "../parser/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ms_result_t ms_vm_exec_string(ms_vm_t* vm, const char* source) {
    ms_chunk_t chunk;
    ms_chunk_init(&chunk);
    
    if (!ms_compile(source, &chunk)) {
        ms_chunk_free(&chunk);
        return MS_RESULT_COMPILE_ERROR;
    }
    
    ms_result_t result = ms_vm_interpret(vm, &chunk);
    ms_chunk_free(&chunk);
    return result;
}

ms_result_t ms_vm_exec_file(ms_vm_t* vm, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        snprintf(vm->error_message, sizeof(vm->error_message), 
                "Could not open file \"%s\".", filename);
        vm->has_error = true;
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    
    char* buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        snprintf(vm->error_message, sizeof(vm->error_message), 
                "Not enough memory to read \"%s\".", filename);
        vm->has_error = true;
        fclose(file);
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        snprintf(vm->error_message, sizeof(vm->error_message), 
                "Could not read file \"%s\".", filename);
        vm->has_error = true;
        free(buffer);
        fclose(file);
        return MS_RESULT_RUNTIME_ERROR;
    }
    
    buffer[bytes_read] = '\0';
    
    fclose(file);
    ms_result_t result = ms_vm_exec_string(vm, buffer);
    free(buffer);
    return result;
}

void ms_vm_register_function(ms_vm_t* vm, const char* name, ms_native_fn_t func) {
    ms_global_t* global = malloc(sizeof(ms_global_t));
    global->name = malloc(strlen(name) + 1);
    strcpy(global->name, name);
    global->value = ms_value_native_func(func);
    global->next = vm->globals;
    vm->globals = global;
}

void ms_vm_set_global(ms_vm_t* vm, const char* name, ms_value_t value) {
    // 查找现有全局变量
    ms_global_t* current = vm->globals;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    
    // 创建新的全局变量
    ms_global_t* global = malloc(sizeof(ms_global_t));
    global->name = malloc(strlen(name) + 1);
    strcpy(global->name, name);
    global->value = value;
    global->next = vm->globals;
    vm->globals = global;
}

ms_value_t ms_vm_get_global(ms_vm_t* vm, const char* name) {
    ms_global_t* current = vm->globals;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return ms_value_nil();
}