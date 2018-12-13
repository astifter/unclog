#define _GNU_SOURCE

#include "unclog_int.h"

#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_INITIALIZER;
static unclog_global_t* unclog_global = NULL;

unclog_t* unclog_open(const char* source) {
    pthread_rwlock_wrlock(&unclog_mutex);

    if (unclog_global == NULL) {
        unclog_global = unclog_global_create();
    }

    unclog_source_t* handle = unclog_source_create(source);
    handle->next = unclog_global->sources;
    unclog_global->sources = handle;

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
	sprintf(buffer, "<%c> %s: %s:%d %s\n", unclog_level_tochar(level), handle->source, file, line, fmt);
		
	va_list al;
	va_start(al, fmt);
	vfprintf(stderr, buffer, al);
	va_end(al);
}

void unclog_close(unclog_t* public_handle) {
    GET_UNCLOG_HANDLE(public_handle);

    pthread_rwlock_wrlock(&unclog_mutex);

    unclog_source_t* s = unclog_global->sources;
    unclog_source_t* p = NULL;
    while (s != NULL) {
        if (s == handle) {
            if (p == NULL) {
                unclog_global->sources = s->next;
            } else {
                p->next = s->next;
            }
			unclog_source_destroy(s);
            break;
        }
		p = s;
		s = s->next;
    }

    if (unclog_global->sources == NULL) {
		unclog_global_destroy(unclog_global);
	}

    pthread_rwlock_unlock(&unclog_mutex);
}
