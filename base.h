#ifndef _BASE_H_
#define _BASE_H_

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#if !defined(O_CLOEXEC) && defined(__O_CLOEXEC)
#define O_CLOEXEC __O_CLOEXEC  
#endif

#define BOOL unsigned char
#define BOOL_BOUND 2
#define TRUE 1
#define FALSE 0


#define BYTE unsigned char
#define __packed __attribute__((packed))

#define FREE(o)                             \
do{                                         \
    if(o){                                  \
        free(o);                            \
        o = NULL;                           \
    }                                       \
}while(0)

#define CHECK(expr)                                         \
do{                                                         \
    if ((!(expr))) {                                        \
        fprintf(stderr, "failed at %s:%d\n",                \
            __FILE__, __LINE__);                            \
        abort();                                            \
    }                                                       \
}while(0)

#define CHECK_LOG(s, expr)                                  \
do{                                                         \
    if((!(expr))) {                                         \
        fprintf(stderr, "failed at %s:%d\n",                \
            __FILE__, __LINE__);                            \
        perror(s);                                          \
        abort();                                            \
    }                                                       \
}while(0)

#ifdef DEBUG
#define DCHECK(expr) CHECK(expr)
#else
#define DCHECK(expr) ((void)0)
#endif

#endif

