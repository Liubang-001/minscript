#ifndef HTTP_EXT_H
#define HTTP_EXT_H

#include "ext.h"
#include <stdint.h>

// HTTP请求结构
typedef struct {
    char* url;
    char* method;
    char* body;
    int timeout;
} ms_http_request_t;

// HTTP响应结构
typedef struct {
    int status_code;
    char* body;
    char* headers;
} ms_http_response_t;

// 异步回调类型
typedef void (*ms_http_callback_t)(ms_http_response_t* response, void* userdata);

// HTTP扩展API
ms_extension_t* ms_http_extension_create(void);
void ms_http_extension_destroy(ms_extension_t* ext);

// 异步I/O事件循环
void ms_http_event_loop_init(void);
void ms_http_event_loop_run(void);
void ms_http_event_loop_stop(void);
void ms_http_event_loop_cleanup(void);

#endif // HTTP_EXT_H
