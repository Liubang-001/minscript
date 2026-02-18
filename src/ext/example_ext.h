#ifndef EXAMPLE_EXT_H
#define EXAMPLE_EXT_H

#include "ext.h"

// 创建示例扩展
ms_extension_t* ms_extension_create(void);

// 销毁示例扩展
void ms_extension_destroy(ms_extension_t* ext);

#endif // EXAMPLE_EXT_H
