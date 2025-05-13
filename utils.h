#ifndef _UTILS_H_
#define _UTILS_H_

#include "base.h"
#include "binder.h"

typedef void (*PROCESS_FUNC)();
typedef void* (*THREAD_FUNC)(void* args);

uint32_t register_binder_service(
    PBINDER_INFO info, 
    const char* name
);

uint32_t search_binder_service(
    PBINDER_INFO info,
    const char* name
);

void hex_print(
    void* buffer, 
    size_t rsize
);
int create_process(
    PROCESS_FUNC child, 
    PROCESS_FUNC parent);

int create_thread(
    THREAD_FUNC func,
    void* args
);

#endif