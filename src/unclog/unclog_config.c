#define _BSD_SOURCE

#include "unclog_int.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MATCH(n, v) (strcmp(n, v) == 0)

static void unclog_level_handler(unclog_values_t* v, const char* value) {
    v->level = unclog_level_tolevel(value);
}

static void unclog_details_handler(unclog_values_t* v, const char* value) {
    char* buffer = strdup(value);
    char* token_save = NULL;
    char* token = strtok_r(buffer, ",", &token_save);
    v->details = 0;
    while (token != NULL) {
        v->details |= unclog_details_todetail(token);
        token = strtok_r(NULL, ",", &token_save);
    }
    free(buffer);
}

static int unclog_common_handler(unclog_values_t* v, const char* name, const char* value) {
    if (MATCH(name, "Level")) {
        unclog_level_handler(v, value);
    } else if (MATCH(name, "Details")) {
        unclog_details_handler(v, value);
    } else {
        return 0;
    }
    return 1;
}

static void unclog_defaults_handler(void* g, const char* name, const char* value) {
    unclog_global_t* global = g;
    if (unclog_common_handler(&global->defaults, name, value)) return;
    if (MATCH(name, "Sinks")) {
        char* buffer = strdup(value);
        char* token_save = NULL;
        char* token = strtok_r(buffer, ",", &token_save);
        while (token != NULL) {
            unclog_sink_t* sink = unclog_sink_create(&global->defaults, token);
            unclog_global_sink_add(g, sink);
            token = strtok_r(NULL, ",", &token_save);
        }
        free(buffer);

        global->sinks_defined = 1;
    }
}

int unclog_ini_handler(void* g, const char* section, const char* name, const char* value) {
    unclog_global_t* global = g;

    fprintf(stderr, "%s.%s: %s\n", section, name, value);
    if (MATCH(section, "Defaults")) {
        unclog_defaults_handler(global, name, value);
        return 0;
    }

    unclog_sink_t* sink = unclog_global_sink_get(global, section);
    if (sink != NULL) {
        if (!unclog_common_handler(&sink->common, name, value))
            unclog_sink_add_keyvalue(sink, name, value);
        return 0;
    }

    unclog_source_t* source = unclog_source_create(&global->defaults, section);
    source->level = unclog_level_tolevel(value);
    unclog_global_source_add(global, source);

    return 0;
}
