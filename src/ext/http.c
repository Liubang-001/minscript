#include "http.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define ASYNC_IO_IOCP 1
#elif defined(__linux__)
    #include <sys/epoll.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define ASYNC_IO_EPOLL 1
#elif defined(__APPLE__)
    #include <sys/event.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define ASYNC_IO_KQUEUE 1
#endif

// 异步请求结构
typedef struct {
    ms_http_request_t request;
    ms_http_response_t response;
    int socket;
    char* buffer;
    int buffer_size;
    int bytes_received;
} ms_http_async_request_t;

// 事件循环状态
typedef struct {
    int running;
    int max_requests;
    ms_http_async_request_t* requests;
    int request_count;
    
#ifdef ASYNC_IO_IOCP
    HANDLE completion_port;
#elif defined(ASYNC_IO_EPOLL)
    int epoll_fd;
#elif defined(ASYNC_IO_KQUEUE)
    int kqueue_fd;
#endif
} ms_http_event_loop_t;

static ms_http_event_loop_t event_loop = {0};

// HTTP扩展函数
static ms_value_t http_get(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* url = ms_value_as_string(args[0]);
    
    // Return a response string
    char response[512];
    snprintf(response, sizeof(response), "GET %s - Status: 200 OK", url);
    
    return ms_value_string(response);
}

static ms_value_t http_post(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 2 || !ms_value_is_string(args[0]) || !ms_value_is_string(args[1])) {
        return ms_value_nil();
    }
    
    const char* url = ms_value_as_string(args[0]);
    const char* body = ms_value_as_string(args[1]);
    
    // Return a response string
    char response[512];
    snprintf(response, sizeof(response), "POST %s - Body: %s - Status: 201 Created", url, body);
    
    return ms_value_string(response);
}

static ms_value_t http_request(ms_vm_t* vm, int argc, ms_value_t* args) {
    if (argc < 1 || !ms_value_is_string(args[0])) {
        return ms_value_nil();
    }
    
    const char* url = ms_value_as_string(args[0]);
    const char* method = (argc > 1 && ms_value_is_string(args[1])) ? ms_value_as_string(args[1]) : "GET";
    
    // Return a response string
    char response[512];
    snprintf(response, sizeof(response), "%s %s - Status: 200 OK", method, url);
    
    return ms_value_string(response);
}

// 初始化事件循环
void ms_http_event_loop_init(void) {
    event_loop.running = 0;
    event_loop.max_requests = 256;
    event_loop.request_count = 0;
    event_loop.requests = malloc(sizeof(ms_http_async_request_t) * event_loop.max_requests);
    
#ifdef ASYNC_IO_IOCP
    event_loop.completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
#elif defined(ASYNC_IO_EPOLL)
    event_loop.epoll_fd = epoll_create1(EPOLL_CLOEXEC);
#elif defined(ASYNC_IO_KQUEUE)
    event_loop.kqueue_fd = kqueue();
#endif
}

// 运行事件循环
void ms_http_event_loop_run(void) {
    event_loop.running = 1;
    
#ifdef ASYNC_IO_IOCP
    // Windows IOCP实现
    OVERLAPPED_ENTRY entries[64];
    ULONG count;
    
    while (event_loop.running) {
        if (GetQueuedCompletionStatusEx(event_loop.completion_port, entries, 64, &count, 100, FALSE)) {
            for (ULONG i = 0; i < count; i++) {
                // 处理完成的I/O操作
            }
        }
    }
#elif defined(ASYNC_IO_EPOLL)
    // Linux epoll实现
    struct epoll_event events[64];
    
    while (event_loop.running) {
        int nfds = epoll_wait(event_loop.epoll_fd, events, 64, 100);
        for (int i = 0; i < nfds; i++) {
            // 处理事件
        }
    }
#elif defined(ASYNC_IO_KQUEUE)
    // macOS kqueue实现
    struct kevent events[64];
    struct timespec timeout = {0, 100000000}; // 100ms
    
    while (event_loop.running) {
        int nfds = kevent(event_loop.kqueue_fd, NULL, 0, events, 64, &timeout);
        for (int i = 0; i < nfds; i++) {
            // 处理事件
        }
    }
#endif
}

// 停止事件循环
void ms_http_event_loop_stop(void) {
    event_loop.running = 0;
}

// 清理事件循环
void ms_http_event_loop_cleanup(void) {
#ifdef ASYNC_IO_IOCP
    if (event_loop.completion_port) {
        CloseHandle(event_loop.completion_port);
    }
#elif defined(ASYNC_IO_EPOLL)
    if (event_loop.epoll_fd >= 0) {
        close(event_loop.epoll_fd);
    }
#elif defined(ASYNC_IO_KQUEUE)
    if (event_loop.kqueue_fd >= 0) {
        close(event_loop.kqueue_fd);
    }
#endif
    
    if (event_loop.requests) {
        free(event_loop.requests);
    }
}

// 创建HTTP扩展
ms_extension_t* ms_http_extension_create(void) {
    ms_extension_t* ext = malloc(sizeof(ms_extension_t));
    ext->name = "http";
    ext->function_count = 3;
    
    ext->functions[0].name = "get";
    ext->functions[0].func = http_get;
    
    ext->functions[1].name = "post";
    ext->functions[1].func = http_post;
    
    ext->functions[2].name = "request";
    ext->functions[2].func = http_request;
    
    return ext;
}

// 销毁HTTP扩展
void ms_http_extension_destroy(ms_extension_t* ext) {
    if (ext) {
        free(ext);
    }
}
