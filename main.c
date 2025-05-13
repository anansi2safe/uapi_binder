#include "binder.h"
#include "utils.h"

#define COOKIE 0x424242
#define HANDLE 0xbeef


void virtual_parent(){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    //BYTE rbuffer[1024];
    //size_t rsize = 1024;
    NEW_TR_BUILDER(tb);    
    NEW_BINDER_OBJECT_BUILDER(bob);
    printf("pid: %d\n", getppid());
    int r = binder_open(info_ptr, 128*1024);
    if(r <= BINDER_VERSION_ERROR){
        fprintf(stderr, "binder driver open failed: %x", r);
        return;
    }
    BYTE rbuffer[1024];
    BYTE buffer[1024];
    uint16_t name[] = {'a','n','d','r','o','i','d','.','c','o','m','.','a','a','p','k','\0'};
    size_t name_len = sizeof(name);
    memset(rbuffer, 0, 1024);
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
    size_t d = binder_transaction(info_ptr, rbuffer, 1024, tb.tr_);
    hex_print(rbuffer, d);
    binder_close(info_ptr);
}

int main(int argc, char* argv[]){
    virtual_parent();
    //return create_process(virtual_client, virtual_parent);
}