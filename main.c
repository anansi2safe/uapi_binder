#include "binder.h"
#include "utils.h"

#define COOKIE 0x424242
#define HANDLE 0xbeef

//自编译的binder没有servicemanager，需要自己编写一个模拟
void virtual_client(){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
        perror("binder version error");
    }
    CHECK_LOG("binder context manager failed!", 
        binder_become_context_manager(
            &info, 0x414141, (binder_uintptr_t)NULL, FALSE) >= 0);
    printf("self: %d\n", getpid());
    getchar();
    binder_close(info_ptr);
}


void virtual_parent(){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    BYTE rbuffer[1024];
    size_t rsize = 1024;
    NEW_TR_BUILDER(tb);    
    NEW_BINDER_OBJECT_BUILDER(bob);
    printf("pid: %d", getppid());
    if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
        perror("binder version error");
    }
    binder_size_t offsets[1] = {0};
    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_BINDER);
    bob.set_fbo_cookie_(&bob, COOKIE);
    bob.set_fbo_handle_(&bob, HANDLE);
    bob.set_fbo_binder_(&bob, 0);
    bob.set_fbo_flags_(&bob, TF_UPDATE_TXN);
    
    tb.set_tr_flags_(&tb, TF_UPDATE_TXN|TF_ONE_WAY);
    tb.set_tr_target_handle_(&tb, search_binder_service(info_ptr, ""));
    tb.set_tr_code_(&tb, SVC_MGR_ADD_SERVICE);
    tb.set_tr_data_size_(&tb, sizeof(bob.obj_));
    tb.set_tr_offsets_size_(&tb, sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)&offsets);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&bob.obj_);
    tb.set_tr_cookie_(&tb, COOKIE);

    for(int i=0; i<2; i++){
        binder_transaction(info_ptr, rbuffer, rsize, tb.tr_);
    }
    binder_close(info_ptr);
}

int main(int argc, char* argv[]){
    virtual_parent();
    //return create_process(virtual_client, virtual_servicemanager);
}