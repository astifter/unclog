#include "unclog.h"

typedef struct unclog_s {
    const char* source;
} unclog_t;

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct unclog_global_s {
    size_t handles;
} unclog_global_t;

static pthread_mutex_t unclog_mutex = PTHREAD_MUTEX_INITIALIZER;
static unclog_global_t* unclog_global = NULL;

unclog_pub_t* unclog_open(const char* source) {
    if(unclog_global == NULL) {
        pthread_mutex_lock(&unclog_mutex);
        if(unclog_global == NULL) {
            unclog_global = malloc(sizeof(unclog_global_t));
        }
        pthread_mutex_unlock(&unclog_mutex);
    }
    unclog_pub_t* handle = malloc(sizeof(unclog_pub_t));
    handle->__internal = malloc(sizeof(unclog_t));
    return handle;
}

#define UNCLOG_HANDLE(ha) if (ha == NULL) return; unclog_pub_t* hp = (ha); unclog_t* h = hp->__internal;

void unclog_log(unclog_pub_t* handle,
                unsigned int level,
                const char* file, const char* func, unsigned int line,
                const char* fmt, ...) {
    UNCLOG_HANDLE(handle);
}

void unclog_close(unclog_pub_t* handle) {
    UNCLOG_HANDLE(handle);
}
