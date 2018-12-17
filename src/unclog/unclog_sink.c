#define _BSD_SOURCE

#include "unclog_int.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void unclog_sink_stderr(unclog_data_int_t* data, va_list list) {
    struct tm time;
    gmtime_r(&data->now.tv_sec, &time);

    char timebuffer[64];
    strftime(timebuffer, 64, "%Y-%m-%d %H:%M:%S", &time);

    const char* fmt = va_arg(list, char*);
    char buffer[PATH_MAX];
    sprintf(buffer, "%s <%c> %s: %s:%d %s\n", timebuffer, unclog_level_tochar(data->le),
            ((unclog_source_t*)data->ha)->source, data->fi, data->li, fmt);

    vfprintf(stderr, buffer, list);
}

typedef struct unclog_sink_list_s {
    const char* name;
    unclog_sink_log_t log;
} unclog_sink_list_t;

static unclog_sink_list_t unclog_default_sinks[] = {
    {"libunclog_stderr.so", unclog_sink_stderr}, {NULL, NULL},
};

unclog_sink_t* unclog_sink_create(unclog_values_t* defaults, const char* sink) {
    unclog_sink_t* handle = malloc(sizeof(unclog_sink_t));
    memset(handle, 0, sizeof(unclog_sink_t));

    memcpy(&handle->common, defaults, sizeof(unclog_values_t));
    handle->sink = strdup(sink);

    unclog_sink_list_t* l = unclog_default_sinks;
    for (; l->name != NULL; l++) {
        if (strcmp(l->name, sink) == 0) {
            handle->log = l->log;
        }
    }

    return handle;
}

void unclog_sink_add_keyvalue(unclog_sink_t* sink, const char* key, const char* value) {
    unclog_keyvalue_t* kv = malloc(sizeof(unclog_keyvalue_t));
    kv->key = strdup(key);
    kv->value = strdup(value);

    kv->next = sink->values;
    sink->values = kv;
}

void unclog_sink_destroy(unclog_sink_t* sink) {
    unclog_keyvalue_t* kv = sink->values;
    while (kv != NULL) {
        unclog_keyvalue_t* d = kv;
        kv = kv->next;
        free(d->key);
        free(d->value);
        free(d);
    }
    free(sink->sink);
    free(sink);
}
