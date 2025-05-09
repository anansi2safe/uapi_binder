#include "utils.h"

uint32_t register_binder_service(
    PBINDER_INFO info, 
    const char* name
){
    uint32_t handle = 0;
    
    return handle;
}

uint32_t search_binder_service(
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
        CHECK_LOG("process create failed!", FALSE);
        return -1;
    }else if (pid == 0){
        child();
    }else{
        parent();
    }
    return 0;
}

int create_thread(
    THREAD_FUNC func,
    void* args
){
    pthread_t thread_id;
    int ret = 0;
    ret = pthread_create(& thread_id, NULL, func, args);
    ret = pthread_join(thread_id, NULL);
    return ret;
}