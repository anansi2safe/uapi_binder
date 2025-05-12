#include "binder.h"
#include "utils.h"

#define COOKIE 0x424242
#define HANDLE 0xbeef

//自编译的binder没有servicemanager，需要自己编写一个模拟
void virtual_client(){
    sleep(1);
    
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

    binder_freeze(info_ptr, getppid(), TRUE, 2000);
    BYTE buffer[1024];
    uint16_t name[] = {'a','n','d','r','o','i','d','.','c','o','m','.','a','a','p','k','\0'};
    size_t name_len = sizeof(name);
    memset(buffer, 0, 1024);
    memcpy(buffer, name, name_len);

    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_HANDLE);
    bob.set_fbo_cookie_(&bob, COOKIE);
    bob.set_fbo_handle_(&bob, HANDLE);
    bob.set_fbo_flags_(&bob, TF_UPDATE_TXN);
    
    memcpy(buffer+name_len+2, &bob.obj_, sizeof(bob.obj_));
    binder_size_t offsets[] = {name_len+2};

    tb.set_tr_flags_(&tb, TF_ONE_WAY|TF_UPDATE_TXN);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_data_size_(&tb, 1024);
    tb.set_tr_offsets_size_(&tb, sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)&offsets);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&buffer);
    tb.set_tr_cookie_(&tb, COOKIE);

    binder_transaction(info_ptr, rbuffer, rsize, tb.tr_);
    binder_close(info_ptr);
}

void handle_binder(
    uint32_t cmd, 
    struct binder_transaction_data tr,
    binder_uintptr_t secctx,
    BOOL is_sec_ctx
){
    printf("\ncookie: %llx\n", tr.cookie);
    printf("\nhandle: %x\n", tr.target.handle);
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

    BYTE buffer[1024];
    uint16_t name[] = {'a','n','d','r','o','i','d','.','c','o','m','.','a','a','p','k','\0'};
    size_t name_len = sizeof(name);
    memset(buffer, 0, 1024);
    memcpy(buffer, name, name_len);

    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_BINDER);
    bob.set_fbo_cookie_(&bob, COOKIE);
    bob.set_fbo_handle_(&bob, HANDLE);
    bob.set_fbo_flags_(&bob, TF_UPDATE_TXN);
    
    memcpy(buffer+name_len+2, &bob.obj_, sizeof(bob.obj_));
    binder_size_t offsets[] = {name_len+2};

    tb.set_tr_flags_(&tb, TF_UPDATE_TXN);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_ADD_SERVICE);
    tb.set_tr_data_size_(&tb, 1024);
    tb.set_tr_offsets_size_(&tb, sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)&offsets);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&buffer);
    tb.set_tr_cookie_(&tb, COOKIE);
    binder_transaction(info_ptr, rbuffer, rsize, tb.tr_);
    //binder_parse(info_ptr, rbuffer, rsize, handle_binder);
    binder_close(info_ptr);
}

int main(int argc, char* argv[]){
    virtual_parent();
    //return create_process(virtual_client, virtual_parent);
}