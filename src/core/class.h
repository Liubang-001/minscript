#ifndef MS_CLASS_H
#define MS_CLASS_H

#include "miniscript.h"

// 类对象
typedef struct ms_class {
    char* name;
    struct ms_class* parent;
    ms_dict_t* methods;
} ms_class_t;

// 实例对象
typedef struct ms_instance {
    ms_class_t* klass;
    ms_dict_t* attrs;
} ms_instance_t;

// 绑定方法
typedef struct ms_bound_method {
    ms_value_t receiver;
    ms_value_t method;
} ms_bound_method_t;

// 类操作
ms_class_t* ms_class_new(const char* name);
void ms_class_free(ms_class_t* klass);

// 实例操作
ms_instance_t* ms_instance_new(ms_class_t* klass);
void ms_instance_free(ms_instance_t* instance);

// 绑定方法操作
ms_bound_method_t* ms_bound_method_new(ms_value_t receiver, ms_value_t method);
void ms_bound_method_free(ms_bound_method_t* bound);

#endif // MS_CLASS_H
