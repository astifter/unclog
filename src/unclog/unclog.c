#define _GNU_SOURCE

#include "unclog_int.h"

#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_INITIALIZER;
unclog_global_t* unclog_global = NULL;

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

void unclog_log(unclog_data_t data, ...) {
    if (data.ha == NULL) return;

    if (data.le > data.ha->level) return;

    unclog_data_int_t di;
    memcpy(&di, &data, sizeof(unclog_data_t));
    clock_gettime(CLOCK_REALTIME, &di.now);

    pthread_rwlock_rdlock(&unclog_mutex);
    unclog_sink_t* sink = unclog_global->sinks;
    for (; sink != NULL; sink = sink->next) {
        if (sink->log == NULL) continue;
        if (data.le > sink->common.level) continue;

        va_list al;
        va_start(al, data);
        di.sink = sink;
        sink->log(&di, al);
        va_end(al);
    }
    pthread_rwlock_unlock(&unclog_mutex);
}

void unclog_close(unclog_t* public_handle) {
    if (public_handle == NULL) return;
    unclog_source_t* handle = (unclog_source_t*)public_handle;

    pthread_rwlock_wrlock(&unclog_mutex);

    handle->active--;
    int has_active_handles;
    if (handle->active == 0) {
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
