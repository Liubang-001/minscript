#include "miniscript.h"
#include "ext/ext.h"
#include "ext/http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void repl(ms_vm_t* vm) {
    char line[1024];
    
    for (;;) {
        printf("> ");
        
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        
        ms_result_t result = ms_vm_exec_string(vm, line);
        if (result == MS_RESULT_COMPILE_ERROR) {
            printf("Compile error\n");
        } else if (result == MS_RESULT_RUNTIME_ERROR) {
            printf("Runtime error: %s\n", ms_vm_get_error(vm));
            ms_vm_clear_error(vm);
        }
    }
}

static void run_file(ms_vm_t* vm, const char* path) {
    ms_result_t result = ms_vm_exec_file(vm, path);
    
    if (result == MS_RESULT_COMPILE_ERROR) {
        fprintf(stderr, "Compile error\n");
        exit(65);
    }
    if (result == MS_RESULT_RUNTIME_ERROR) {
        fprintf(stderr, "Runtime error: %s\n", ms_vm_get_error(vm));
        exit(70);
    }
}

// 内置函数示例
static ms_value_t builtin_print(ms_vm_t* vm, int argc, ms_value_t* args) {
    for (int i = 0; i < argc; i++) {
        if (i > 0) printf(" ");
        ms_value_t value = args[i];
        switch (value.type) {
            case MS_VAL_BOOL:
                printf(ms_value_as_bool(value) ? "True" : "False");
                break;
            case MS_VAL_NIL: printf("None"); break;
            case MS_VAL_INT: printf("%lld", (long long)ms_value_as_int(value)); break;
            case MS_VAL_FLOAT: printf("%g", ms_value_as_float(value)); break;
            case MS_VAL_STRING: printf("%s", ms_value_as_string(value)); break;
            default: printf("<object>"); break;
        }
    }
    printf("\n");
    return ms_value_nil();
}

static ms_value_t builtin_len(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc != 1) {
        return ms_value_nil();
    }
    
    if (ms_value_is_string(args[0])) {
        return ms_value_int(strlen(ms_value_as_string(args[0])));
    }
    
    return ms_value_nil();
}

int main(int argc, const char* argv[]) {
    ms_vm_t* vm = ms_vm_new();
    
    // 注册内置函数 (Python 3 style)
    ms_vm_register_function(vm, "print", builtin_print);
    ms_vm_register_function(vm, "len", builtin_len);
    
    // 注册扩展
    ms_extension_t* http_ext = ms_http_extension_create();
    ms_register_extension(vm, http_ext);
    
    if (argc == 1) {
        printf("MiniScript v%d.%d.%d (Python 3 syntax)\n", 
               MS_VERSION_MAJOR, MS_VERSION_MINOR, MS_VERSION_PATCH);
        repl(vm);
    } else if (argc == 2) {
        run_file(vm, argv[1]);
    } else {
        fprintf(stderr, "Usage: miniscript [path]\n");
        exit(64);
    }
    
    ms_http_extension_destroy(http_ext);
    ms_vm_free(vm);
    return 0;
}