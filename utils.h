#ifndef _UTILS_H_
#define _UTILS_H_

#include "base.h"
#include "binder.h"

typedef void (*PROCESS_FUNC)();

uint32_t register_binder_service(
    PBINDER_INFO info, 
    const char* name);

int create_process(
    PROCESS_FUNC child, 
    PROCESS_FUNC parent);

#endif