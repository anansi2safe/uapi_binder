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

BYTE* set_data_service_name(
    BYTE* ptr, 
    const uint16_t* name, 
    uint32_t len
){
    *((uint32_t*)ptr) = len;
    memcpy(ptr+sizeof(uint32_t), name, len);
    return ptr+sizeof(uint32_t)+len;
}

BYTE* set_data_service_fbo(
    BYTE* ptr,
    BINDER_OBJECT_BUILDER b
){
    memcpy(ptr, &b.obj_, sizeof(b.obj_));
    return ptr+sizeof(b.obj_);
}

void register_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    uint32_t name_len,
    binder_uintptr_t cookie,
    uint32_t handle
){
    DCHECK(name_len < MAX_SERVICE_NAME);
    struct{
        struct{
            uint32_t strict_policy_;
            uint32_t worksource_head_;
            uint32_t mgr_name_len_;
            uint16_t mgr_name_[26];
        }data_head_;
        BYTE data_ptr_[512];
    }__packed data;

    NEW_TR_BUILDER(tb);    
    NEW_BINDER_OBJECT_BUILDER(bob);
    BYTE rbuffer[1024];
    size_t rsize = 1024;
    memset(rbuffer, 0, 1024);

    BYTE* tmp_ptr = data.data_ptr_;
    init_register_data(data, 0, 0);
    tmp_ptr = set_data_service_name(tmp_ptr, name, name_len);

    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_BINDER);
    bob.set_fbo_cookie_(&bob, cookie);
    bob.set_fbo_handle_(&bob, handle);
    bob.set_fbo_flags_(&bob, TF_ACCEPT_FDS);
    binder_size_t offset1 = tmp_ptr - data.data_ptr_;
    tmp_ptr = set_data_service_fbo(tmp_ptr, bob);
    binder_size_t offsets[] = {offset1};

    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_ADD_SERVICE);
    tb.set_tr_data_size_(&tb, sizeof(data));
    tb.set_tr_offsets_size_(&tb, sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)&offsets);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&data);
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