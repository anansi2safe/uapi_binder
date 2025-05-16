#ifndef _UTILS_H_
#define _UTILS_H_

#include "base.h"
#include "binder.h"

typedef void (*PROCESS_FUNC)();
typedef void* (*THREAD_FUNC)(void* args);

void register_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len,
    binder_uintptr_t cookie,
    uint32_t handle
);

uint32_t get_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len
);

int create_process(PROCESS_FUNC func);


#endif