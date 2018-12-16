#define _GNU_SOURCE

#include "unclog_int.h"

#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_INITIALIZER;
static unclog_global_t* unclog_global = NULL;

unclog_t* unclog_open(const char* source) {
    pthread_rwlock_wrlock(&unclog_mutex);

    if (unclog_global == NULL) {
        unclog_global = unclog_global_create();
    }

    unclog_source_t* handle = unclog_global_source_get(unclog_global, source);
    if (handle == NULL) {
        handle = unclog_source_create(&unclog_global->defaults, source);
        unclog_global_source_add(unclog_global, handle);
    }
    handle->active++;

    pthread_rwlock_unlock(&unclog_mutex);

    return (unclog_t*)handle;
}

#define GET_UNCLOG_HANDLE(ha) \
    if (ha == NULL) return;   \
    unclog_source_t* handle = (unclog_source_t*)(ha);

void unclog_log(unclog_t* public_handle, unsigned int level, const char* file, const char* func,
                unsigned int line, const char* fmt, ...) {
    GET_UNCLOG_HANDLE(public_handle);
    char buffer[PATH_MAX] = {0};

    (void)func;
    sprintf(buffer, "<%c> %s: %s:%d %s\n", unclog_level_tochar(level), handle->source, file, line,
            fmt);

    va_list al;
    va_start(al, fmt);
    vfprintf(stderr, buffer, al);
    va_end(al);
}

void unclog_close(unclog_t* public_handle) {
    GET_UNCLOG_HANDLE(public_handle);

    pthread_rwlock_wrlock(&unclog_mutex);

    handle->active--;
    int has_active_handles;
    if(handle->active == 0) {
        has_active_handles = unclog_global_source_remove(unclog_global, handle);
    } else {
        has_active_handles = 1;
    }

    if (!has_active_handles) {
        unclog_global_destroy(unclog_global);
        unclog_global = NULL;
    }

    pthread_rwlock_unlock(&unclog_mutex);
}
