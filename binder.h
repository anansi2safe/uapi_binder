#ifndef _BINDER_H_
#define _BINDER_H_

#include "base.h"
#include "uapi_binder.h"

#define BINDER_DEVICE "/dev/binder"

#define BINDER_SUCCESS 0
#define BINDER_VERSION_ERROR -15
#define BINDER_MAPPED_ERROR (BINDER_VERSION_ERROR - 1)

#define FIELD_DEFINE(name, arg) \
void (*name)(struct __TR_BUILDER* this, arg)

#define SET_FIELD(o, name) \
o.name##_ = name

typedef struct __BINDER_INFO
{
    int fd_;
    void* mapped_;
    size_t mapsize_;
}BINDER_INFO, *PBINDER_INFO;

typedef struct __TR_BUILDER{
    struct binder_transaction_data tr_;
    FIELD_DEFINE(set_tr_target_handle_, uint32_t handle);
    FIELD_DEFINE(set_tr_target_ptr_, binder_uintptr_t ptr);
    FIELD_DEFINE(set_tr_cookie_, binder_uintptr_t cookie);
    FIELD_DEFINE(set_tr_code_, uint32_t code);
    FIELD_DEFINE(set_tr_flags_, uint32_t flags);
    FIELD_DEFINE(set_tr_sender_pid_, pid_t sender_pid);
    FIELD_DEFINE(set_tr_sender_euid_, uid_t sender_euid);
    FIELD_DEFINE(set_tr_data_size_, binder_size_t data_size);
    FIELD_DEFINE(set_tr_offsets_size_, binder_size_t offsets_size);
    FIELD_DEFINE(set_tr_data_ptr_buffer_, binder_uintptr_t buffer);
    FIELD_DEFINE(set_tr_data_ptr_offsets_, binder_uintptr_t offsets);
}TR_BUILDER, *PTR_BUILDER;

#define NEW_TR_BUILDER(o)                   \
TR_BUILDER o;                               \
do{                                         \
    SET_FIELD(o, set_tr_target_handle);     \
    SET_FIELD(o, set_tr_target_ptr);        \
    SET_FIELD(o, set_tr_cookie);            \
    SET_FIELD(o, set_tr_code);              \
    SET_FIELD(o, set_tr_flags);             \
    SET_FIELD(o, set_tr_sender_pid);        \
    SET_FIELD(o, set_tr_sender_euid);       \
    SET_FIELD(o, set_tr_data_size);         \
    SET_FIELD(o, set_tr_offsets_size);      \
    SET_FIELD(o, set_tr_data_ptr_buffer);   \
    SET_FIELD(o, set_tr_data_ptr_offsets);  \
}while(0)

int binder_read(PBINDER_INFO info, BYTE* buffer, size_t size);
int binder_write(PBINDER_INFO info, BYTE* buffer, size_t size);
int binder_become_context_manager(PBINDER_INFO info, BOOL ext);
int binder_open(PBINDER_INFO info, size_t mapsize);
int binder_close(PBINDER_INFO info);
int binder_read_write(
    PBINDER_INFO info, 
    BYTE* wbuffer, 
    size_t wsize, 
    BYTE* rbuffer, 
    size_t rsize
);
int binder_transaction(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
);
int binder_transaction_sg(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
);

/**
 * TR_BUILDER members function
 */
void set_tr_target_handle(
    PTR_BUILDER this,
    uint32_t handle
);

void set_tr_target_ptr(
    PTR_BUILDER this,
    binder_uintptr_t ptr
);

void set_tr_cookie(
    PTR_BUILDER this,
    binder_uintptr_t cookie
);

void set_tr_code(
    PTR_BUILDER this,
    uint32_t code
);

void set_tr_flags(
    PTR_BUILDER this,
    uint32_t flags
);

void set_tr_sender_pid(
    PTR_BUILDER this,
    pid_t sender_pid
);

void set_tr_sender_euid(
    PTR_BUILDER this,
    uid_t sender_euid
);

void set_tr_data_size(
    PTR_BUILDER this,
    binder_size_t data_size
);

void set_tr_offsets_size(
    PTR_BUILDER this,
    binder_size_t offsets_size
);

void set_tr_data_ptr_buffer(
    PTR_BUILDER this,
    binder_uintptr_t buffer
);

void set_tr_data_ptr_offsets(
    PTR_BUILDER this,
    binder_uintptr_t offsets
);

#endif