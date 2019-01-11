#define _BSD_SOURCE

#include "unclog_int.h"

unclog_source_t* unclog_source_create(int level, const char* source) {
    unclog_source_t* handle = malloc(sizeof(unclog_source_t));
    memset(handle, 0, sizeof(unclog_source_t));

    handle->source = strdup(source);
    handle->level = level;

    return handle;
}

void unclog_source_destroy(unclog_source_t* source) {
    free(source->source);
    free(source);
}
