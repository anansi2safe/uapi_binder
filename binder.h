#ifndef _BINDER_H_
#define _BINDER_H_

#include "base.h"
#include "uapi_binder.h"

#define BINDER_DEVICE "/dev/binder"
#define DEFAULT_MAX_BINDER_THREADS 0xFF

#define BINDER_SUCCESS 0
#define BINDER_VERSION_ERROR -15
#define BINDER_MAPPED_ERROR (BINDER_VERSION_ERROR - 1)
#define BINDER_SET_MAX_THREADS_ERROR (BINDER_MAPPED_ERROR - 1)

#define TR_FIELD_DEFINE(name, arg) \
void (*name)(struct __TR_BUILDER* this, arg)

#define BINDER_OBJECT_FIELD_DEFINE(name, arg) \
void (*name)(struct __BINDER_OBJECT_BUILDER* this, arg)

#define SET_FIELD(o, name) \
o.name##_ = name

typedef void (*BINDER_PARSE_CALLBACK)(
    uint32_t, 
    struct binder_transaction_data,
    binder_uintptr_t,
    BOOL
);

enum {
    /* Must match definitions in IBinder.h and IServiceManager.h */
    PING_TRANSACTION  = B_PACK_CHARS('_','P','N','G'),
    SVC_MGR_GET_SERVICE = 1,
    SVC_MGR_CHECK_SERVICE,
    SVC_MGR_ADD_SERVICE,
    SVC_MGR_LIST_SERVICES,
};

typedef struct __BINDER_INFO
{
    int fd_;
    void* mapped_;
    size_t mapsize_;
}BINDER_INFO, *PBINDER_INFO;


typedef struct __BINDER_OBJECT_BUILDER{
    struct flat_binder_object obj_;
    BINDER_OBJECT_FIELD_DEFINE(set_fbo_flags_, uint32_t flags);
    BINDER_OBJECT_FIELD_DEFINE(set_fbo_cookie_, binder_uintptr_t cookie);
    BINDER_OBJECT_FIELD_DEFINE(set_fbo_binder_, binder_uintptr_t binder);
    BINDER_OBJECT_FIELD_DEFINE(set_fbo_handle_, uint32_t handle);
    BINDER_OBJECT_FIELD_DEFINE(set_fbo_hdr_type_, uint32_t type);
}BINDER_OBJECT_BUILDER, *PBINDER_OBJECT_BUILDER;

typedef struct __TR_BUILDER{
    struct binder_transaction_data tr_;
    TR_FIELD_DEFINE(set_tr_target_handle_, uint32_t handle);
    TR_FIELD_DEFINE(set_tr_target_ptr_, binder_uintptr_t ptr);
    TR_FIELD_DEFINE(set_tr_cookie_, binder_uintptr_t cookie);
    TR_FIELD_DEFINE(set_tr_code_, uint32_t code);
    TR_FIELD_DEFINE(set_tr_flags_, uint32_t flags);
    TR_FIELD_DEFINE(set_tr_sender_pid_, pid_t sender_pid);
    TR_FIELD_DEFINE(set_tr_sender_euid_, uid_t sender_euid);
    TR_FIELD_DEFINE(set_tr_data_size_, binder_size_t data_size);
    TR_FIELD_DEFINE(set_tr_offsets_size_, binder_size_t offsets_size);
    TR_FIELD_DEFINE(set_tr_data_ptr_buffer_, binder_uintptr_t buffer);
    TR_FIELD_DEFINE(set_tr_data_ptr_offsets_, binder_uintptr_t offsets);
}TR_BUILDER, *PTR_BUILDER;

#define NEW_TR_BUILDER(o)                   \
TR_BUILDER o;                               \
memset(&o, 0, sizeof(o));                   \
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

#define NEW_BINDER_OBJECT_BUILDER(o)        \
BINDER_OBJECT_BUILDER o;                    \
memset(&o, 0, sizeof(o));                   \
do{                                         \
    SET_FIELD(o, set_fbo_binder);           \
    SET_FIELD(o, set_fbo_cookie);           \
    SET_FIELD(o, set_fbo_flags);            \
    SET_FIELD(o, set_fbo_handle);           \
    SET_FIELD(o, set_fbo_hdr_type);         \
}while(0)

size_t binder_read(PBINDER_INFO info, BYTE* buffer, size_t size);
int binder_write(PBINDER_INFO info, BYTE* buffer, size_t size);
int binder_increfs(PBINDER_INFO info, uint32_t target);
int binder_acquire(PBINDER_INFO info, uint32_t target);
int binder_release(PBINDER_INFO info, uint32_t target);
int binder_decrefs(PBINDER_INFO info, uint32_t target);
int binder_open(PBINDER_INFO info, size_t mapsize);
int binder_close(PBINDER_INFO info);


int binder_free_buffer(
    PBINDER_INFO info, 
    binder_uintptr_t buffer
);

int binder_freeze(
    PBINDER_INFO info,
    uint32_t pid,
    uint32_t enable,
    uint32_t timeout_ms
);

int binder_request_death_notification(
    PBINDER_INFO info, 
    uint32_t target, 
    binder_uintptr_t cookie);

int binder_clear_death_notification(
    PBINDER_INFO info, 
    uint32_t target, 
    binder_uintptr_t cookie);

int binder_become_context_manager(
    PBINDER_INFO info, 
    binder_uintptr_t cookie, 
    binder_uintptr_t binder,
    BOOL ext);

size_t binder_read_write(
    PBINDER_INFO info, 
    BYTE* wbuffer, 
    size_t wsize, 
    BYTE* rbuffer, 
    size_t rsize
);
size_t binder_transaction(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
);

size_t binder_transaction_sg(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
);

int binder_reply(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
);

int binder_reply_sg(
    PBINDER_INFO info, 
    BYTE* rbuffer,
    size_t rsize,
    struct binder_transaction_data tr
);

uint32_t binder_parse(
    PBINDER_INFO info, 
    BYTE* rbuffer, 
    size_t rsize,
    BINDER_PARSE_CALLBACK callback
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

/**
 * BINDER_OBJECT_BUILDER members function
 */
void set_fbo_flags(
    PBINDER_OBJECT_BUILDER this,
    uint32_t flags
);

void set_fbo_cookie(
    PBINDER_OBJECT_BUILDER this,
    binder_uintptr_t cookie
);

void set_fbo_binder(
    PBINDER_OBJECT_BUILDER this,
    binder_uintptr_t binder
);

void set_fbo_handle(
    PBINDER_OBJECT_BUILDER this,
    uint32_t handle
);

void set_fbo_hdr_type(
    PBINDER_OBJECT_BUILDER this,
    uint32_t type
);
#endif
