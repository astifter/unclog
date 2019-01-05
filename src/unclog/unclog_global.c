#include "unclog_int.h"

#include <ini.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char* unclog_ini_files[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

void unclog_global_dump_config(unclog_global_t* global) {
    fprintf(stderr, "global->defaults.level: %s\n", unclog_level_tostr(global->defaults.level));
    char* detailsstr = unclog_details_tostr(global->defaults.details);
    fprintf(stderr, "global->defaults.details: %s\n", detailsstr);
    free(detailsstr);
    unclog_source_t* source = global->sources;
    for (; source != NULL; source = source->next) {
        fprintf(stderr, "global->source[%s].level: %s\n", source->source,
                unclog_level_tostr(source->level));
    }
    unclog_sink_t* sink = global->sinks;
    for (; sink != NULL; sink = sink->next) {
        fprintf(stderr, "global->sink[%s].level: %s\n", sink->sink,
                unclog_level_tostr(sink->settings.level));
        detailsstr = unclog_details_tostr(sink->settings.details);
        fprintf(stderr, "global->sink[%s].details: %04X | %s\n", sink->sink, sink->settings.details,
                detailsstr);
        free(detailsstr);
        unclog_keyvalue_t* kv = sink->values;
        for (; kv != NULL; kv = kv->next) {
            fprintf(stderr, "global->sink[%s].%s: %s\n", sink->sink, kv->key, kv->value);
        }
    }
}

unclog_global_t* unclog_global_create(const char* config, int usefile, int initialized) {
    unclog_global_t* g = malloc(sizeof(unclog_global_t));
    memset(g, 0, sizeof(unclog_global_t));
    memcpy(&g->defaults, &unclog_defaults, sizeof(unclog_values_t));
    g->initialized = initialized;

    if (usefile == 1) {
        const char** f = unclog_ini_files;
        for (; *f != NULL; f++) {
            if (access(*f, R_OK) != 0) continue;
            ini_parse(*f, unclog_ini_handler, g);
            break;
        }
    } else {
        if (config != NULL) {
            ini_parse_string(config, unclog_ini_handler, g);
        }
    }

    if (g->sinks_defined == 0) {
        unclog_sink_t* s = unclog_sink_create(&g->defaults, "libunclog_stderr.so");
        unclog_global_sink_add(g, s);
    }

    return g;
}

void* unclog_global_destroy(unclog_global_t* global) {
    unclog_sink_t* sink = global->sinks;
    while (sink != NULL) {
        unclog_sink_t* d = sink;
        sink = sink->next;
        unclog_sink_destroy(d);
    }
    unclog_source_t* source = global->sources;
    while (source != NULL) {
        unclog_source_t* d = source;
        source = source->next;
        unclog_source_destroy(d);
    }

    free(global);

    return NULL;
}

void unclog_global_source_add(unclog_global_t* global, unclog_source_t* source) {
    source->next = global->sources;
    global->sources = source;
}

unclog_source_t* unclog_global_source_get(unclog_global_t* global, const char* source) {
    unclog_source_t* s = global->sources;
    for (; s != NULL; s = s->next) {
        if (strcmp(s->source, source) == 0) return s;
    }
    return NULL;
}

int unclog_global_source_remove(unclog_global_t* global, unclog_source_t* source) {
    int has_active_handles = 0;
    unclog_source_t* s = global->sources;
    unclog_source_t* p = NULL;
    while (s != NULL) {
        if (s == source) {
            if (p == NULL) {
                global->sources = s->next;
            } else {
                p->next = s->next;
            }
            unclog_source_t* d = s;
            s = s->next;
            unclog_source_destroy(d);
        } else {
            if (s->active == 1) has_active_handles = 1;
            p = s;
            s = s->next;
        }
    }
    return has_active_handles;
}

void unclog_global_sink_add(unclog_global_t* global, unclog_sink_t* source) {
    source->next = global->sinks;
    global->sinks = source;
}

unclog_sink_t* unclog_global_sink_get(unclog_global_t* global, const char* sink) {
    unclog_sink_t* s = global->sinks;
    for (; s != NULL; s = s->next) {
        if (strcmp(s->sink, sink) == 0) return s;
    }
    return NULL;
}
