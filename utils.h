#ifndef _UTILS_H_
#define _UTILS_H_

#include "base.h"
#include "binder.h"

typedef void (*PROCESS_FUNC)();
typedef void* (*THREAD_FUNC)(void* args);

uint32_t binder_parse_log(
    BYTE* rbuffer, 
    size_t rsize
);

void register_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    uint32_t name_len,
    binder_uintptr_t cookie,
    uint32_t handle,
    uint32_t strict_policy,
    uint32_t worksource_header,
    uint32_t allow_isolated,
    uint32_t dumpsys_priority

);

uint32_t get_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len,
    binder_uintptr_t cookie,
    uint32_t strict_policy,
    uint32_t worksource_header
);

int create_process(PROCESS_FUNC func);

void print_hex(BYTE* buffer, size_t len);
#endif