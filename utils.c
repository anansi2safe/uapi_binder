#include "utils.h"

const uint16_t svcmgr_id[] = {
    'a','n','d','r','o','i','d','.',
    'o','s','.', 'I','S','e','r','v',
    'i','c','e', 'M','a','n','a','g',
    'e','r'}; 

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
    uint32_t name_len,
    binder_uintptr_t cookie,
    uint32_t handle,
    uint32_t strict_policy,
    uint32_t worksource_header,
    uint32_t allow_isolated,
    uint32_t dumpsys_priority

){
    DCHECK(name_len < MAX_SERVICE_NAME);

    NEW_TR_BUILDER(tb);    
    NEW_BINDER_OBJECT_BUILDER(bob);
    BYTE rbuffer[1024];
    BYTE wbuffer[1024];
    size_t rsize = 1024;
    size_t wsize = 1024;
    memset(rbuffer, 0, rsize);
    memset(wbuffer, 0, wsize);

    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_BINDER);
    bob.set_fbo_cookie_(&bob, cookie);
    bob.set_fbo_handle_(&bob, handle);
    bob.set_fbo_flags_(&bob, TF_ACCEPT_FDS);
    
    /**
     * register data:
     * |-----strict_policy(32bits)-----|
     * |---worksource_header(32bits)---|
     * |-----svcmgr_id len(32bits)-----|
     * |------svcmgr_id(26bits)+'\0'---|
     * |------svc_name len(32bits)-----|
     * |------svc_name(*bits)+'\0'-----|
     * |---flat_binder_object(*bits)---|
     * |-----allow_isolated(32bits)----|
     * |----dumpsys_priority(32bits)---|
     * 
     * 注：
     * 所有的name len都是其实际字节大小除以2,并且不包括'\0'，但是实际拷贝时要空出'\0'(uint16_t)的位置
     */
    /*strict_policy*/
    size_t tmp_off = 0;
    *((uint32_t*)(wbuffer+tmp_off)) = strict_policy;
    tmp_off += sizeof(uint32_t);
    /*worksource_header*/
    *((uint32_t*)(wbuffer+tmp_off)) = worksource_header;
    tmp_off += sizeof(uint32_t);
    /*svcmgr_id len*/
    *((uint32_t*)(wbuffer+tmp_off)) = sizeof(svcmgr_id) / 0x2;
    tmp_off += sizeof(uint32_t);
    /*svcmgr_id*/
    memcpy(wbuffer+tmp_off, svcmgr_id, sizeof(svcmgr_id));
    tmp_off += sizeof(svcmgr_id) + 0x4;
    /*svc_name len*/
    *((uint32_t*)(wbuffer+tmp_off)) = name_len / 0x2;
    tmp_off += sizeof(uint32_t);
    /*svc_name*/
    memcpy(wbuffer+tmp_off, name, name_len);
    tmp_off += name_len + 0x4;
    /*flat_binder_object*/
    binder_size_t offset1 = tmp_off;
    memcpy(wbuffer+tmp_off, &bob.obj_, sizeof(bob.obj_));
    tmp_off += sizeof(bob.obj_);
    /*allow_isolated and dumpsys_priority*/
    *((uint32_t*)(wbuffer+tmp_off)) = allow_isolated;
    tmp_off += sizeof(uint32_t);
    *((uint32_t*)(wbuffer+tmp_off)) = dumpsys_priority;

    binder_size_t offsets[] = {offset1};

    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_ADD_SERVICE);
    tb.set_tr_data_size_(&tb, wsize);
    tb.set_tr_offsets_size_(&tb, sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)&offsets);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)wbuffer);
    tb.set_tr_cookie_(&tb, cookie);
    size_t d = binder_transaction(info, rbuffer, 1024, tb.tr_);
    print_hex(rbuffer, d);
    binder_parse_log(rbuffer, d);
}

uint32_t get_binder_service(
    PBINDER_INFO info, 
    const uint16_t* name,
    size_t name_len,
    uint32_t strict_policy,
    uint32_t worksource_header
){
    uint32_t handle = 0;
    NEW_TR_BUILDER(tb);
    BYTE rbuffer[1024];
    BYTE wbuffer[1024];
    size_t rsize = 1024;
    size_t wsize = 1024;
    
    memset(rbuffer, 0, 1024);
    memset(wbuffer, 0, 1024);

    /*strict_policy*/
    size_t tmp_off = 0;
    *((uint32_t*)(wbuffer+tmp_off)) = strict_policy;
    tmp_off += sizeof(uint32_t);
    /*worksource_header*/
    *((uint32_t*)(wbuffer+tmp_off)) = worksource_header;
    tmp_off += sizeof(uint32_t);
    /*svcmgr_id len*/
    *((uint32_t*)(wbuffer+tmp_off)) = sizeof(svcmgr_id) / 0x2;
    tmp_off += sizeof(uint32_t);
    /*svcmgr_id*/
    memcpy(wbuffer+tmp_off, svcmgr_id, sizeof(svcmgr_id));
    tmp_off += sizeof(svcmgr_id) + 0x4;
    /*svc_name len*/
    *((uint32_t*)(wbuffer+tmp_off)) = name_len / 0x2;
    tmp_off += sizeof(uint32_t);
    /*svc_name*/
    memcpy(wbuffer+tmp_off, name, name_len);

    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_target_handle_(&tb, 0);
    tb.set_tr_code_(&tb, SVC_MGR_GET_SERVICE);
    tb.set_tr_data_size_(&tb, wsize);
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)wbuffer);
    size_t d = binder_transaction(info, rbuffer, rsize, tb.tr_);
    binder_parse_log(rbuffer, d);
    print_hex(rbuffer, d);
    return handle;
}

void print_hex(BYTE* buffer, size_t len){
    uint32_t* na = (uint32_t*)buffer;
    for(size_t i=0; i<len; i++){
        printf("%x ", na[i]);
    }
    puts("\n");
}