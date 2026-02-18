#include "string_ext.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// String extension functions
static ms_value_t string_upper(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    char* result = malloc(strlen(str) + 1);
    
    for (int i = 0; str[i]; i++) {
        result[i] = toupper((unsigned char)str[i]);
    }
    result[strlen(str)] = '\0';
    
    ms_value_t ret = ms_value_string(result);
    free(result);
    return ret;
}

static ms_value_t string_lower(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    char* result = malloc(strlen(str) + 1);
    
    for (int i = 0; str[i]; i++) {
        result[i] = tolower((unsigned char)str[i]);
    }
    result[strlen(str)] = '\0';
    
    ms_value_t ret = ms_value_string(result);
    free(result);
    return ret;
}

static ms_value_t string_len(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    return ms_value_int((int64_t)strlen(str));
}

static ms_value_t string_reverse(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    int len = strlen(str);
    char* result = malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    
    ms_value_t ret = ms_value_string(result);
    free(result);
    return ret;
}

static ms_value_t string_startswith(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 2 || !ms_value_is_string(args[0]) || !ms_value_is_string(args[1])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    const char* prefix = ms_value_as_string(args[1]);
    
    return ms_value_bool(strncmp(str, prefix, strlen(prefix)) == 0);
}

static ms_value_t string_endswith(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 2 || !ms_value_is_string(args[0]) || !ms_value_is_string(args[1])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    const char* suffix = ms_value_as_string(args[1]);
    
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return ms_value_bool(false);
    }
    
    return ms_value_bool(strcmp(str + str_len - suffix_len, suffix) == 0);
}

static ms_value_t string_contains(ms_vm_t* vm, int argc, ms_value_t* args) {
    (void)vm;
    if (argc < 2 || !ms_value_is_string(args[0]) || !ms_value_is_string(args[1])) {
        return ms_value_nil();
    }
    
    const char* str = ms_value_as_string(args[0]);
    const char* substr = ms_value_as_string(args[1]);
    
    return ms_value_bool(strstr(str, substr) != NULL);
}

// Create string extension
ms_extension_t* ms_string_extension_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "string";
    ext->function_count = 7;
    
    ext->functions[0].name = "upper";
    ext->functions[0].func = string_upper;
    
    ext->functions[1].name = "lower";
    ext->functions[1].func = string_lower;
    
    ext->functions[2].name = "len";
    ext->functions[2].func = string_len;
    
    ext->functions[3].name = "reverse";
    ext->functions[3].func = string_reverse;
    
    ext->functions[4].name = "startswith";
    ext->functions[4].func = string_startswith;
    
    ext->functions[5].name = "endswith";
    ext->functions[5].func = string_endswith;
    
    ext->functions[6].name = "contains";
    ext->functions[6].func = string_contains;
    
    return ext;
}

// Destroy string extension
void ms_string_extension_destroy(ms_extension_t* ext) {
    if (ext) {
        free(ext);
    }
}
