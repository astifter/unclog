#include "unclog_int.h"

#include <ini.h>

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
    for (; sink != NULL; sink = sink->i->next) {
        fprintf(stderr, "global->sink[%s].level: %s\n", sink->i->sink,
                unclog_level_tostr(sink->settings.level));
        detailsstr = unclog_details_tostr(sink->settings.details);
        fprintf(stderr, "global->sink[%s].details: %04X | %s\n", sink->i->sink,
                sink->settings.details, detailsstr);
        free(detailsstr);
        unclog_keyvalue_t* kv = sink->values;
        for (; kv != NULL; kv = kv->next) {
            fprintf(stderr, "global->sink[%s].%s: %s\n", sink->i->sink, kv->key, kv->value);
        }
    }
}

void unclog_global_configure(unclog_global_t* global, const char* config, int usefile,
                             int initialized) {
    memcpy(&global->defaults, &unclog_defaults, sizeof(unclog_values_t));
    global->sinks_defined = 0;
    global->initialized = initialized;
    for (unclog_sink_t* s = global->sinks; s != NULL; s = s->i->next) {
        memcpy(&s->settings, &unclog_defaults, sizeof(unclog_values_t));
        for (unclog_keyvalue_t* k = s->values; k != NULL; k = k->next) {
            unclog_keyvalue_destroy(k);
        }
    }
    for (unclog_source_t* s = global->sources; s != NULL; s = s->next) {
        s->level = unclog_defaults.level;
    }

    if (usefile == 1) {
        const char** f = unclog_ini_files;
        for (; *f != NULL; f++) {
            if (access(*f, R_OK) != 0) continue;
            ini_parse(*f, unclog_ini_handler, global);
            break;
        }
    } else {
        if (config != NULL) {
            ini_parse_string(config, unclog_ini_handler, global);
        }
    }

    if (global->sinks_defined == 0) {
        unclog_sink_t* s = unclog_sink_create(&global->defaults, "stderr");
        unclog_global_sink_add(global, s);
    }

    for (unclog_sink_t* s = global->sinks; s != NULL; s = s->i->next) {
        if (s->i->methods.init != NULL) s->i->methods.init(s);
    }
}

unclog_global_t* unclog_global_create(const char* config, int usefile, int initialized) {
    unclog_global_t* g = malloc(sizeof(unclog_global_t));
    memset(g, 0, sizeof(unclog_global_t));
    pthread_mutex_init(&g->now_mutex, NULL);

    unclog_global_configure(g, config, usefile, initialized);
    return g;
}

void unclog_global_sink_clear(unclog_global_t* global, int include_registered) {
    unclog_sink_t* s = global->sinks;
    unclog_sink_t* p = NULL;

    while (s != NULL) {
        if (s->i->registered == 0 || include_registered) {
            if (p == NULL) {
                global->sinks = s->i->next;
            } else {
                p->i->next = s->i->next;
            }
            unclog_sink_t* d = s;
            s = s->i->next;
            unclog_sink_destroy(d);
        } else {
            p = s;
            s = s->i->next;
        }
    }
}

void unclog_global_destroy(unclog_global_t* global) {
    unclog_global_sink_clear(global, 1);

    unclog_source_t* source = global->sources;
    while (source != NULL) {
        unclog_source_t* d = source;
        source = source->next;
        unclog_source_destroy(d);
    }

    pthread_mutex_destroy(&global->now_mutex);
    free(global);
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

void unclog_global_sink_add(unclog_global_t* global, unclog_sink_t* sink) {
    sink->i->next = global->sinks;
    global->sinks = sink;
}

unclog_sink_t* unclog_global_sink_get(unclog_global_t* global, const char* sink) {
    unclog_sink_t* s = global->sinks;
    for (; s != NULL; s = s->i->next) {
        if (strcmp(s->i->sink, sink) == 0) return s;
    }
    return NULL;
}
