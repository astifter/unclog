#define _BSD_SOURCE

#include "unclog_int.h"

size_t stringappend(char* dest, const char* src) {
    size_t size = strlen(src);
    memcpy(dest, src, size + 1);
    return size;
}

static void unclog_sink_stderr(unclog_data_t* data, va_list list) {
    char buffer[PATH_MAX] = {0};
    char* bufferpos = buffer;

    uint32_t details = data->si->settings.details;
    uint32_t needsspace = 0;

    if (details & UNCLOG_OPT_TIMESTAMP) {
        struct tm time;
        gmtime_r(&data->no.tv_sec, &time);

        char timebuffer[64];
        int size = strftime(timebuffer, 64, "%Y-%m-%d %H:%M:%S", &time);

        memcpy(buffer, timebuffer, size);
        bufferpos += size;
        needsspace = 1;
    }
    if (details & UNCLOG_OPT_LEVEL) {
        if (needsspace) *(bufferpos++) = ' ';
        bufferpos += stringappend(bufferpos, "< >");
        *(bufferpos - 2) = unclog_level_tochar(data->le);
        needsspace = 1;
    }
    if (details & UNCLOG_OPT_SOURCE) {
        if (needsspace) *(bufferpos++) = ' ';
        bufferpos += stringappend(bufferpos, ((unclog_source_t*)data->ha)->source);
        needsspace = 1;
    }
    if (details & UNCLOG_OPT_FILE) {
        if (needsspace) *(bufferpos++) = ' ';
        bufferpos += stringappend(bufferpos, data->fi);
        needsspace = 1;
    }
    if (details & UNCLOG_OPT_LINE) {
        if (needsspace) {
            if (details & UNCLOG_OPT_FILE)
                *(bufferpos++) = ':';
            else
                *(bufferpos++) = ' ';
        }
        bufferpos += sprintf(bufferpos, "%d", data->li);
        needsspace = 1;
    }
    if (details & UNCLOG_OPT_MESSAGE) {
        if (needsspace) *(bufferpos++) = ' ';
        const char* fmt = va_arg(list, char*);
        bufferpos += vsprintf(bufferpos, fmt, list);
        needsspace = 1;
    }
    *bufferpos = '\n';
    bufferpos++;

    fprintf(stderr, buffer);
}

typedef struct unclog_sink_list_s {
    const char* name;
    unclog_sink_methods_t methods;
} unclog_sink_list_t;

static unclog_sink_list_t unclog_default_sinks[] = {
    {"stderr", {NULL, unclog_sink_stderr, NULL}}, {NULL, {NULL}},
};

unclog_sink_t* unclog_sink_create(unclog_values_t* settings, const char* name) {
    unclog_sink_t* handle = malloc(sizeof(unclog_sink_t));
    memset(handle, 0, sizeof(unclog_sink_t));
    handle->i = malloc(sizeof(unclog_sink_internal_t));
    memset(handle->i, 0, sizeof(unclog_sink_internal_t));

    if (settings != NULL)
        memcpy(&handle->settings, settings, sizeof(unclog_values_t));
    else
        memcpy(&handle->settings, &unclog_global->defaults, sizeof(unclog_values_t));
    handle->i->sink = strdup(name);

    unclog_sink_list_t* l = unclog_default_sinks;
    for (; l->name != NULL; l++) {
        if (strcmp(l->name, name) == 0) {
            handle->i->methods = l->methods;
        }
    }

    return handle;
}

unclog_keyvalue_t* unclog_keyvalue_create(const char* key, const char* value) {
    unclog_keyvalue_t* kv = malloc(sizeof(unclog_keyvalue_t));
    kv->key = strdup(key);
    kv->value = strdup(value);

    return kv;
}

void unclog_keyvalue_destroy(unclog_keyvalue_t* v) {
    free(v->key);
    free(v->value);
    free(v);
}

void unclog_sink_add_keyvalue(unclog_sink_t* sink, const char* key, const char* value) {
    unclog_keyvalue_t* kv = unclog_keyvalue_create(key, value);

    kv->next = sink->values;
    sink->values = kv;
}

void unclog_sink_destroy(unclog_sink_t* sink) {
    if (sink->i->methods.deinit != NULL) sink->i->methods.deinit(sink);

    unclog_keyvalue_t* kv = sink->values;
    while (kv != NULL) {
        unclog_keyvalue_t* d = kv;
        kv = kv->next;
        unclog_keyvalue_destroy(d);
    }
    free(sink->i->sink);
    free(sink->i);
    free(sink);
}
