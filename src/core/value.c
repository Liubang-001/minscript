#include "miniscript.h"
#include <string.h>
#include <stdlib.h>

// 创建值
ms_value_t ms_value_nil(void) {
    ms_value_t value;
    value.type = MS_VAL_NIL;
    return value;
}

ms_value_t ms_value_bool(bool val) {
    ms_value_t value;
    value.type = MS_VAL_BOOL;
    value.as.boolean = val;
    return value;
}

ms_value_t ms_value_int(int64_t val) {
    ms_value_t value;
    value.type = MS_VAL_INT;
    value.as.integer = val;
    return value;
}

ms_value_t ms_value_float(double val) {
    ms_value_t value;
    value.type = MS_VAL_FLOAT;
    value.as.floating = val;
    return value;
}

ms_value_t ms_value_string(const char* str) {
    ms_value_t value;
    value.type = MS_VAL_STRING;
    value.as.string = malloc(strlen(str) + 1);
    strcpy(value.as.string, str);
    return value;
}

ms_value_t ms_value_native_func(ms_native_fn_t func) {
    ms_value_t value;
    value.type = MS_VAL_NATIVE_FUNC;
    value.as.native_func = malloc(sizeof(ms_native_func_t));
    value.as.native_func->func = func;
    value.as.native_func->name = NULL;
    return value;
}

// 类型检查
bool ms_value_is_nil(ms_value_t value) {
    return value.type == MS_VAL_NIL;
}

bool ms_value_is_bool(ms_value_t value) {
    return value.type == MS_VAL_BOOL;
}

bool ms_value_is_int(ms_value_t value) {
    return value.type == MS_VAL_INT;
}

bool ms_value_is_float(ms_value_t value) {
    return value.type == MS_VAL_FLOAT;
}

bool ms_value_is_string(ms_value_t value) {
    return value.type == MS_VAL_STRING;
}

bool ms_value_is_function(ms_value_t value) {
    return value.type == MS_VAL_FUNCTION || value.type == MS_VAL_NATIVE_FUNC;
}

// 值转换
bool ms_value_as_bool(ms_value_t value) {
    switch (value.type) {
        case MS_VAL_BOOL: return value.as.boolean;
        case MS_VAL_NIL: return false;
        case MS_VAL_INT: return value.as.integer != 0;
        case MS_VAL_FLOAT: return value.as.floating != 0.0;
        case MS_VAL_STRING: return strlen(value.as.string) > 0;
        default: return true;
    }
}

int64_t ms_value_as_int(ms_value_t value) {
    switch (value.type) {
        case MS_VAL_INT: return value.as.integer;
        case MS_VAL_FLOAT: return (int64_t)value.as.floating;
        case MS_VAL_BOOL: return value.as.boolean ? 1 : 0;
        default: return 0;
    }
}

double ms_value_as_float(ms_value_t value) {
    switch (value.type) {
        case MS_VAL_FLOAT: return value.as.floating;
        case MS_VAL_INT: return (double)value.as.integer;
        case MS_VAL_BOOL: return value.as.boolean ? 1.0 : 0.0;
        default: return 0.0;
    }
}

const char* ms_value_as_string(ms_value_t value) {
    if (value.type == MS_VAL_STRING) {
        return value.as.string;
    }
    return "";
}