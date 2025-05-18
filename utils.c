#include "utils.h"

int create_process(PROCESS_FUNC func){
    int ret = 0;
    pid_t pid = fork();
    if(pid == 0){
        func();
        exit(0);
    }else if(pid < 0){
        ret = pid;
        return ret;
    }else{
        return ret;
    }
}

void register_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len,
    binder_uintptr_t cookie,
    uint32_t handle
){
    puts("register");
    NEW_TR_BUILDER(tb);    
    NEW_BINDER_OBJECT_BUILDER(bob);
    BYTE rbuffer[1024];
    BYTE buffer[1024];
    size_t rsize = 1024;

    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_BINDER);
    bob.set_fbo_cookie_(&bob, cookie);
    bob.set_fbo_handle_(&bob, handle);
    bob.set_fbo_flags_(&bob, TF_ACCEPT_FDS);

    
    memset(rbuffer, 0, 1024);
    memset(buffer, 0, 1024);
    memcpy(buffer, name, name_len);
    memcpy(buffer+name_len+2, &bob.obj_, sizeof(bob.obj_));

    binder_size_t offsets[] = {name_len+2};
    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_ADD_SERVICE);
    tb.set_tr_data_size_(&tb, 1024);
    tb.set_tr_offsets_size_(&tb, sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)&offsets);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&buffer);
    tb.set_tr_cookie_(&tb, cookie);
    size_t d = binder_transaction(info, rbuffer, 1024, tb.tr_);
    print_hex(rbuffer, d);
    binder_parse_log(rbuffer, d);
}

uint32_t check_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len
){
    uint32_t handle = 0;
    NEW_TR_BUILDER(tb);
    BYTE rbuffer[1024];
    BYTE buffer[1024];
    
    memset(rbuffer, 0, 1024);
    memset(buffer, 0, 1024);
    memcpy(buffer, name, name_len);

    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_CHECK_SERVICE);
    tb.set_tr_data_size_(&tb, 1024);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&buffer);
    size_t d = binder_transaction(info, rbuffer, 1024, tb.tr_);
    binder_parse_log(rbuffer, d);
    print_hex(rbuffer, d);
    BYTE* trp = rbuffer + 0xC;
    struct binder_transaction_data* tr = (struct binder_transaction_data*)trp;
    printf("\nhandle is: %x", tr->target.handle);
    return handle;
}

uint32_t get_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len
){
    uint32_t handle = 0;
    NEW_TR_BUILDER(tb);
    BYTE rbuffer[1024];
    BYTE buffer[1024];
    
    memset(rbuffer, 0, 1024);
    memset(buffer, 0, 1024);
    memcpy(buffer, name, name_len);

    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_GET_SERVICE);
    tb.set_tr_data_size_(&tb, 1024);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&buffer);
    size_t d = binder_transaction(info, rbuffer, 1024, tb.tr_);
    binder_parse_log(rbuffer, d);
    print_hex(rbuffer, d);
    BYTE* trp = rbuffer + 0xC;
    struct binder_transaction_data* tr = (struct binder_transaction_data*)trp;
    printf("\nhandle is: %x", tr->target.handle);
    return handle;
}

void print_hex(BYTE* buffer, size_t len){
    uint32_t* na = (uint32_t*)buffer;
    for(size_t i=0; i<len; i++){
        printf("%x ", na[i]);
    }
    puts("\n");
}