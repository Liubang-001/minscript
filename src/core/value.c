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


// Collection value creation
ms_value_t ms_value_list(ms_list_t* list) {
    ms_value_t value;
    value.type = MS_VAL_LIST;
    value.as.list = list;
    return value;
}

ms_value_t ms_value_dict(ms_dict_t* dict) {
    ms_value_t value;
    value.type = MS_VAL_DICT;
    value.as.dict = dict;
    return value;
}

ms_value_t ms_value_tuple(ms_tuple_t* tuple) {
    ms_value_t value;
    value.type = MS_VAL_TUPLE;
    value.as.tuple = tuple;
    return value;
}

// Collection type checks
bool ms_value_is_list(ms_value_t value) {
    return value.type == MS_VAL_LIST;
}

bool ms_value_is_dict(ms_value_t value) {
    return value.type == MS_VAL_DICT;
}

bool ms_value_is_tuple(ms_value_t value) {
    return value.type == MS_VAL_TUPLE;
}

// Collection value conversions
ms_list_t* ms_value_as_list(ms_value_t value) {
    if (value.type == MS_VAL_LIST) {
        return value.as.list;
    }
    return NULL;
}

ms_dict_t* ms_value_as_dict(ms_value_t value) {
    if (value.type == MS_VAL_DICT) {
        return value.as.dict;
    }
    return NULL;
}

ms_tuple_t* ms_value_as_tuple(ms_value_t value) {
    if (value.type == MS_VAL_TUPLE) {
        return value.as.tuple;
    }
    return NULL;
}

// List operations
ms_list_t* ms_list_new(void) {
    ms_list_t* list = malloc(sizeof(ms_list_t));
    list->elements = NULL;
    list->count = 0;
    list->capacity = 0;
    return list;
}

void ms_list_free(ms_list_t* list) {
    if (list) {
        free(list->elements);
        free(list);
    }
}

void ms_list_append(ms_list_t* list, ms_value_t value) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->elements = realloc(list->elements, list->capacity * sizeof(ms_value_t));
    }
    list->elements[list->count++] = value;
}

ms_value_t ms_list_get(ms_list_t* list, int index) {
    if (index < 0 || index >= list->count) {
        return ms_value_nil();
    }
    return list->elements[index];
}

void ms_list_set(ms_list_t* list, int index, ms_value_t value) {
    if (index >= 0 && index < list->count) {
        list->elements[index] = value;
    }
}

int ms_list_len(ms_list_t* list) {
    return list->count;
}

// Dict operations
ms_dict_t* ms_dict_new(void) {
    ms_dict_t* dict = malloc(sizeof(ms_dict_t));
    dict->entries = NULL;
    dict->count = 0;
    dict->capacity = 0;
    return dict;
}

void ms_dict_free(ms_dict_t* dict) {
    if (dict) {
        for (int i = 0; i < dict->count; i++) {
            free(dict->entries[i].key);
        }
        free(dict->entries);
        free(dict);
    }
}

void ms_dict_set(ms_dict_t* dict, const char* key, ms_value_t value) {
    // Check if key exists
    for (int i = 0; i < dict->count; i++) {
        if (strcmp(dict->entries[i].key, key) == 0) {
            dict->entries[i].value = value;
            return;
        }
    }
    
    // Add new entry
    if (dict->count >= dict->capacity) {
        dict->capacity = dict->capacity == 0 ? 8 : dict->capacity * 2;
        dict->entries = realloc(dict->entries, dict->capacity * sizeof(ms_dict_entry_t));
    }
    
    dict->entries[dict->count].key = malloc(strlen(key) + 1);
    strcpy(dict->entries[dict->count].key, key);
    dict->entries[dict->count].value = value;
    dict->count++;
}

ms_value_t ms_dict_get(ms_dict_t* dict, const char* key) {
    for (int i = 0; i < dict->count; i++) {
        if (strcmp(dict->entries[i].key, key) == 0) {
            return dict->entries[i].value;
        }
    }
    return ms_value_nil();
}

bool ms_dict_has(ms_dict_t* dict, const char* key) {
    for (int i = 0; i < dict->count; i++) {
        if (strcmp(dict->entries[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

int ms_dict_len(ms_dict_t* dict) {
    return dict->count;
}

// Tuple operations
ms_tuple_t* ms_tuple_new(int count) {
    ms_tuple_t* tuple = malloc(sizeof(ms_tuple_t));
    tuple->elements = malloc(count * sizeof(ms_value_t));
    tuple->count = count;
    for (int i = 0; i < count; i++) {
        tuple->elements[i] = ms_value_nil();
    }
    return tuple;
}

void ms_tuple_free(ms_tuple_t* tuple) {
    if (tuple) {
        free(tuple->elements);
        free(tuple);
    }
}

ms_value_t ms_tuple_get(ms_tuple_t* tuple, int index) {
    if (index < 0 || index >= tuple->count) {
        return ms_value_nil();
    }
    return tuple->elements[index];
}

int ms_tuple_len(ms_tuple_t* tuple) {
    return tuple->count;
}
