#include "utils.h"

uint32_t register_binder_service(
    PBINDER_INFO info, 
    const char* name
){
    uint32_t handle = 0;
    
    return handle;
}

int create_process(
    PROCESS_FUNC child, 
    PROCESS_FUNC parent
){
    pid_t pid = fork();
    if(pid < 0){
        return -1;
    }else if (pid == 0){
        child();
    }else{
        parent();
    }
    return 0;
}