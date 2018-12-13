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

    const char** f = unclog_ini_files;
    for (; *f != NULL; f++) {
        if (access(*f, R_OK) != 0) continue;
        ini_parse(*f, unclog_ini_handler, g);
        fprintf(stderr, "Defaults: Level: %d\n", g->defaults.level);
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
