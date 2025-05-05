#include "binder.h"
#include "utils.h"

void virtual_servicemanager(){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
        perror("binder version error");
    }
    binder_become_context_manager(&info, 0x414141, (binder_uintptr_t)NULL, FALSE);
    getchar();
    binder_close(info_ptr);
}

void virtual_client(){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    NEW_TR_BUILDER(tb);    
    if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
        perror("binder version error");
    }

    tb.set_tr_flags_(&tb, TF_ONE_WAY);
    tb.set_tr_target_handle_(&tb, 0);
    binder_increfs(info_ptr, 0);
    binder_transaction(info_ptr, NULL, 0, tb.tr_);
    binder_close(info_ptr);
}

void error(){
    perror("create process failed!");
}

int main(int argc, char* argv[]){

    return create_process(virtual_client, virtual_servicemanager);
}