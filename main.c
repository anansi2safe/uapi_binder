#include "binder.h"

int main(int argc, char* argv[]){
    BINDER_INFO info;
    PBINDER_INFO info_ptr = &info;
    NEW_TR_BUILDER(tb);
    if(binder_open(info_ptr, 128*1024) == BINDER_VERSION_ERROR){
        perror("binder version error");
        return -1;
    }
    binder_close(info_ptr);
    return 0;
}