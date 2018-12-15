#include "unclog_int.h"

#include <ini.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char* unclog_ini_files[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

unclog_global_t* unclog_global_create(void) {
    unclog_global_t* g = malloc(sizeof(unclog_global_t));
    memset(g, 0, sizeof(unclog_global_t));
    g->defaults.level = UNCLOG_LEVEL_DEFAULT;
    g->defaults.options = UNCLOG_OPT_DEFAULTS;

    const char** f = unclog_ini_files;
    for (; *f != NULL; f++) {
        if (access(*f, R_OK) != 0) continue;
        ini_parse(*f, unclog_ini_handler, g);
    }

    fprintf(stderr, "g->defaults.level: %s\n", unclog_level_tostr(g->defaults.level));
    fprintf(stderr, "g->defaults.options: 0x%02x\n", g->defaults.options);
    for (unclog_source_t* s = g->sources; s != NULL; s = s->next) {
        fprintf(stderr, "g->source[%s].level: %s\n", s->source,
                unclog_level_tostr(s->public.level));
    }
    for (unclog_sink_t* s = g->sinks; s != NULL; s = s->next) {
        fprintf(stderr, "g->sink[%s].level: %s\n", s->sink, unclog_level_tostr(s->common.level));
        fprintf(stderr, "g->sink[%s].options: 0x%02x\n", s->sink, s->common.options);
        unclog_keyvalue_t* kv = s->values;
        for (; kv != NULL; kv = kv->next) {
            fprintf(stderr, "g->sink[%s].%s: %s\n", s->sink, kv->key, kv->value);
        }
    }

    return g;
}

void unclog_global_destroy(unclog_global_t* global) { free(global); }

void unclog_global_source_add(unclog_global_t* global, unclog_source_t* handle) {
    handle->next = global->sources;
    global->sources = handle;
}

void unclog_global_source_remove(unclog_global_t* global, unclog_source_t* handle) {
    unclog_source_t* s = global->sources;
    unclog_source_t* p = NULL;
    while (s != NULL) {
        if (s == handle) {
            if (p == NULL) {
                global->sources = s->next;
            } else {
                p->next = s->next;
            }
            unclog_source_destroy(s);
            break;
        }
        p = s;
        s = s->next;
    }
}

void unclog_global_sink_add(unclog_global_t* global, unclog_sink_t* handle) {
    handle->next = global->sinks;
    global->sinks = handle;
}

unclog_sink_t* unclog_global_sink_get(unclog_global_t* global, const char* sink) {
    unclog_sink_t* s = global->sinks;
    for (; s != NULL; s = s->next) {
        if (strcmp(s->sink, sink) == 0) return s;
    }
    return NULL;
}

unclog_source_t* unclog_global_source_get(unclog_global_t* global, const char* source) {
    unclog_source_t* s = global->sources;
    for (; s != NULL; s = s->next) {
        if (strcmp(s->source, source) == 0) return s;
    }
    return NULL;
}
