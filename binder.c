#include "binder.h"


int binder_become_context_manager(
    PBINDER_INFO info, 
    binder_uintptr_t cookie, 
    binder_uintptr_t binder,
    BOOL ext
){
    DCHECK(ext < BOOL_BOUND);
    if(ext){
        struct flat_binder_object fbo;
        fbo.hdr.type = BINDER_TYPE_BINDER;
        fbo.flags = FLAT_BINDER_FLAG_ACCEPTS_FDS;
        fbo.binder = binder;
        fbo.cookie = cookie;

        return ioctl(info->fd_, BINDER_SET_CONTEXT_MGR_EXT, &fbo);
    } else {
        return ioctl(info->fd_, BINDER_SET_CONTEXT_MGR, NULL);
    }
}

int binder_free_buffer(
    PBINDER_INFO info, 
    binder_uintptr_t buffer
){
    struct {
        uint32_t cmd_;
        binder_uintptr_t buffer_;
    }__packed data;
    data.cmd_ = BC_FREE_BUFFER;
    data.buffer_ = buffer;
    return binder_write(info, (BYTE*)&data, sizeof(data));
}

uint32_t binder_parse(
    PBINDER_INFO info, 
    BYTE* rbuffer, 
    size_t rsize,
    BINDER_PARSE_CALLBACK callback
){
    uint32_t result = 0;
    size_t end_addr = (size_t)rbuffer + rsize;
    for (size_t ptr = (size_t)rbuffer; ptr < end_addr; ){
        uint32_t cmd = *((uint32_t*)ptr);
        ptr += sizeof(uint32_t);
        switch (cmd) {
        case BR_NOOP:
            break;
        case BR_TRANSACTION_COMPLETE:
        case BR_ONEWAY_SPAM_SUSPECT:
            result = cmd;
            break;
        case BR_TRANSACTION_SEC_CTX:
        case BR_TRANSACTION:{
            BOOL is_sec_ctx = FALSE;
            struct binder_transaction_data_secctx tds;
            size_t surplus_size = end_addr-ptr;
            memset(&tds, 0, sizeof(tds));
            if(cmd == BR_TRANSACTION){
                CHECK(surplus_size > sizeof(
                    struct binder_transaction_data_secctx));
                memcpy(&tds, (const void*)ptr, sizeof(tds));
                ptr += sizeof(tds);
            }else{
                CHECK(surplus_size > sizeof(
                    struct binder_transaction_data));
                is_sec_ctx = TRUE;
                memcpy(
                    &tds.transaction_data, 
                    (const void*)ptr, sizeof(tds.transaction_data));
            }
            // Maybe need free buffer?
            // Now, let the callback function manually free the buffer
            callback(cmd, tds, is_sec_ctx);
            result = cmd;
            break;
        }
        default:
            break;
        }
    }
    return result;
}

int binder_freeze(
    PBINDER_INFO info,
    uint32_t pid,
    uint32_t enable,
    uint32_t timeout_ms
){
    struct binder_freeze_info bfi;
    bfi.pid = pid;
    bfi.enable = enable;
    bfi.timeout_ms = timeout_ms;
    return ioctl(info->fd_, BINDER_FREEZE, &bfi);
}

int binder_open(PBINDER_INFO info, size_t mapsize){
    struct binder_version ver;
    info->fd_ = open(BINDER_DEVICE, O_RDWR | O_CLOEXEC);
    CHECK_LOG("binder device open failed!", info->fd_ >= 0);

    if(ioctl(info->fd_, BINDER_VERSION, &ver) < 0)
        return BINDER_VERSION_ERROR;

    info->mapsize_ = mapsize;
    info->mapped_ = mmap(
        NULL, mapsize, PROT_READ, MAP_PRIVATE, info->fd_, 0);
    if(info->mapped_ == MAP_FAILED)
        return BINDER_MAPPED_ERROR;

    return BINDER_SUCCESS;
}

int binder_close(PBINDER_INFO info){
    munmap(info->mapped_, info->mapsize_);
    close(info->fd_);
    return BINDER_SUCCESS;
}

int binder_increfs(PBINDER_INFO info, uint32_t target){
    struct {
        uint32_t cmd_;
        uint32_t target_;
    }__packed data;

    data.cmd_ = BC_INCREFS;
    data.target_ = target;
    return binder_write(info, (BYTE*)&data, sizeof(data));
}

int binder_acquire(PBINDER_INFO info, uint32_t target){
    struct {
        uint32_t cmd_;
        uint32_t target_;
    }__packed data;

    data.cmd_ = BC_ACQUIRE;
    data.target_ = target;
    return binder_write(info, (BYTE*)&data, sizeof(data));
}

int binder_release(PBINDER_INFO info, uint32_t target){
    struct {
        uint32_t cmd_;
        uint32_t target_;
    }__packed data;

    data.cmd_ = BC_RELEASE;
    data.target_ = target;
    return binder_write(info, (BYTE*)&data, sizeof(data));
}

int binder_decrefs(PBINDER_INFO info, uint32_t target){
    struct {
        uint32_t cmd_;
        uint32_t target_;
    }__packed data;

    data.cmd_ = BC_DECREFS;
    data.target_ = target;
    return binder_write(info, (BYTE*)&data, sizeof(data));
}

int binder_read_write(
    PBINDER_INFO info, 
    BYTE* wbuffer, 
    size_t wsize, 
    BYTE* rbuffer, 
    size_t rsize
){
    struct binder_write_read bwr;
    bwr.read_buffer = (binder_uintptr_t)rbuffer;
    bwr.read_size = rsize;
    bwr.read_consumed = 0;

    bwr.write_buffer = (binder_uintptr_t)wbuffer;
    bwr.write_size = wsize;
    bwr.write_consumed = 0;

    return ioctl(info->fd_, BINDER_WRITE_READ, &bwr);
}

int binder_read(PBINDER_INFO info, BYTE* buffer, size_t size){
    return binder_read_write(info, NULL, 0, buffer, size);
}

int binder_write(PBINDER_INFO info, BYTE* buffer, size_t size){
    return binder_read_write(info, buffer, size, NULL, 0);
}

int binder_transaction(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
){
    struct {
        uint32_t cmd_;
        struct binder_transaction_data tr_;
    }__packed data;

    data.cmd_ = BC_TRANSACTION;
    data.tr_ = tr;
    return binder_read_write(
        info, (BYTE*)&data, sizeof(data), rbuffer, rsize);
}

int binder_transaction_sg(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
){
    struct {
        uint32_t cmd_;
        struct binder_transaction_data_sg tr_;
    }__packed data;

    data.cmd_ = BC_TRANSACTION_SG;
    data.tr_.buffers_size = sizeof(tr);
    data.tr_.transaction_data = tr;
    return binder_read_write(
        info, (BYTE*)&data, sizeof(data), rbuffer, rsize);
}

int binder_reply(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
){
    struct {
        uint32_t cmd_;
        struct binder_transaction_data tr_;
    }__packed data;

    data.cmd_ = BC_REPLY;
    data.tr_ = tr;
    return binder_read_write(
        info, (BYTE*)&data, sizeof(data), rbuffer, rsize);
}

int binder_reply_sg(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
){
    struct {
        uint32_t cmd_;
        struct binder_transaction_data_sg tr_;
    }__packed data;

    data.cmd_ = BC_REPLY_SG;
    data.tr_.buffers_size = sizeof(tr);
    data.tr_.transaction_data = tr;
    return binder_read_write(
        info, (BYTE*)&data, sizeof(data), rbuffer, rsize);
}

int binder_request_death_notification(
    PBINDER_INFO info, 
    uint32_t target, 
    binder_uintptr_t cookie
){
    struct {
        uint32_t cmd_;
        uint32_t target_;
        binder_uintptr_t cookie_;
    }__packed data;

    data.cmd_ = BC_REQUEST_DEATH_NOTIFICATION;
    data.target_ = target;
    data.cookie_ = cookie;

    return binder_read_write(
        info, (BYTE*)&data, sizeof(data), (BYTE*)NULL, 0);
}

int binder_clear_death_notification(
    PBINDER_INFO info, 
    uint32_t target, 
    binder_uintptr_t cookie
){
    struct {
        uint32_t cmd_;
        uint32_t target_;
        binder_uintptr_t cookie_;
    }__packed data;

    data.cmd_ = BC_CLEAR_DEATH_NOTIFICATION;
    data.target_ = target;
    data.cookie_ = cookie;

    return binder_read_write(
        info, (BYTE*)&data, sizeof(data), (BYTE*)NULL, 0);
}

void set_tr_target_handle(
    PTR_BUILDER this,
    uint32_t handle
){
    this->tr_.target.handle = handle;
}

void set_tr_target_ptr(
    PTR_BUILDER this,
    binder_uintptr_t ptr
){
    this->tr_.target.ptr = ptr;
}

void set_tr_cookie(
    PTR_BUILDER this,
    binder_uintptr_t cookie
){
    this->tr_.cookie = cookie;
}

void set_tr_code(
    PTR_BUILDER this,
    uint32_t code
){
    this->tr_.code = code;
}

void set_tr_flags(
    PTR_BUILDER this,
    uint32_t flags
){
    this->tr_.flags = flags;
}

void set_tr_sender_pid(
    PTR_BUILDER this,
    pid_t sender_pid
){
    this->tr_.sender_pid = sender_pid;
}

void set_tr_sender_euid(
    PTR_BUILDER this,
    uid_t sender_euid
){
    this->tr_.sender_euid = sender_euid;
}

void set_tr_data_size(
    PTR_BUILDER this,
    binder_size_t data_size
){
    this->tr_.data_size = data_size;
}

void set_tr_offsets_size(
    PTR_BUILDER this,
    binder_size_t offsets_size
){
    this->tr_.offsets_size = offsets_size;
}

void set_tr_data_ptr_buffer(
    PTR_BUILDER this,
    binder_uintptr_t buffer
){
    this->tr_.data.ptr.buffer = buffer;
}

void set_tr_data_ptr_offsets(
    PTR_BUILDER this,
    binder_uintptr_t offsets
){
    this->tr_.data.ptr.offsets = offsets;
}

void set_fbo_flags(
    PBINDER_OBJECT_BUILDER this,
    uint32_t flags
){
    this->obj_.flags = flags;
}

void set_fbo_cookie(
    PBINDER_OBJECT_BUILDER this,
    binder_uintptr_t cookie
){
    this->obj_.cookie = cookie;
}

void set_fbo_binder(
    PBINDER_OBJECT_BUILDER this,
    binder_uintptr_t binder
){
    this->obj_.binder = binder;
}

void set_fbo_handle(
    PBINDER_OBJECT_BUILDER this,
    uint32_t handle
){
    this->obj_.handle = handle;
}

void set_fbo_hdr_type(
    PBINDER_OBJECT_BUILDER this,
    uint32_t type
){
    this->obj_.hdr.type = type;
}
