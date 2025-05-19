#ifndef _UTILS_H_
#define _UTILS_H_

#include "base.h"
#include "binder.h"

typedef void (*PROCESS_FUNC)();
typedef void* (*THREAD_FUNC)(void* args);

#define set_data_head(d, s, w)                                          \
do{                                                                     \
    uint16_t svcmgr_id[] = {                                            \
        'a','n','d','r','o','i','d','.','o','s','.',                    \
        'I','S','e','r','v','i','c','e','M','a','n','a','g','e','r'     \
    };                                                                  \
    d.data_head_.strict_policy_ = s;                                    \
    d.data_head_.worksource_head_ = w;                                  \
    d.data_head_.mgr_name_len_ = 26;                                    \
    memcpy(                                                             \
        d.data_head_.mgr_name_,                                         \
        svcmgr_id, sizeof(svcmgr_id));                                  \
}while(0)

#define init_register_data(d, s, w)                                     \
do{                                                                     \
    memset(&d, 0, sizeof(d));                                           \
    set_data_head(d, s, w);                                             \
}while(0)

BYTE* set_data_service_name(
    BYTE* ptr, 
    const uint16_t* name, 
    uint32_t len
);

BYTE* set_data_service_fbo(
    BYTE* ptr,
    BINDER_OBJECT_BUILDER b
);

void register_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    uint32_t name_len,
    binder_uintptr_t cookie,
    uint32_t handle
);

uint32_t get_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len
);

uint32_t check_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len
);

int create_process(PROCESS_FUNC func);

void print_hex(BYTE* buffer, size_t len);
#endif