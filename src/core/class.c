#include "class.h"
#include <stdlib.h>
#include <string.h>

ms_class_t* ms_class_new(const char* name) {
    ms_class_t* klass = malloc(sizeof(ms_class_t));
    klass->name = strdup(name);
    klass->parent = NULL;
    klass->methods = ms_dict_new();
    return klass;
}

void ms_class_free(ms_class_t* klass) {
    if (klass) {
        free(klass->name);
        ms_dict_free(klass->methods);
        free(klass);
    }
}

ms_instance_t* ms_instance_new(ms_class_t* klass) {
    ms_instance_t* instance = malloc(sizeof(ms_instance_t));
    instance->klass = klass;
    instance->attrs = ms_dict_new();
    return instance;
}

void ms_instance_free(ms_instance_t* instance) {
    if (instance) {
        ms_dict_free(instance->attrs);
        free(instance);
    }
}

ms_bound_method_t* ms_bound_method_new(ms_value_t receiver, ms_value_t method) {
    ms_bound_method_t* bound = malloc(sizeof(ms_bound_method_t));
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}

void ms_bound_method_free(ms_bound_method_t* bound) {
    if (bound) {
        free(bound);
    }
}
