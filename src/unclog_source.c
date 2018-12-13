#define _BSD_SOURCE

#include "unclog_int.h"

#include <stdlib.h>
#include <string.h>

unclog_source_t* unclog_source_create(unclog_defaults_t* defaults, const char* source) {
    unclog_source_t* handle = malloc(sizeof(unclog_source_t));
    handle->source = strdup(source);
    handle->level = defaults->level;
    return handle;
}

void unclog_source_destroy(unclog_source_t* source) {
    free(source->source);
    free(source);
}
