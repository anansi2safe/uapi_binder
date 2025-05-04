#include "binder.h"


int binder_become_context_manager(PBINDER_INFO info, BOOL ext){
    DCHECK(ext < BOOL_BOUND);
    if(ext){
        struct flat_binder_object fbo;
        fbo.hdr.type = BINDER_TYPE_BINDER;
        fbo.flags = FLAT_BINDER_FLAG_ACCEPTS_FDS;
        fbo.binder = (binder_uintptr_t)NULL;
        fbo.cookie = (binder_uintptr_t)NULL;

        return ioctl(info->fd_, BINDER_SET_CONTEXT_MGR_EXT, &fbo);
    } else {
        return ioctl(info->fd_, BINDER_SET_CONTEXT_MGR_EXT, NULL);
    }
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
