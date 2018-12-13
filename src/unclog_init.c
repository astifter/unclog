#include "unclog_int.h"

#include <string.h>

typedef struct unclog_value_s {
    const char* name;
    void (*handler)(void* user, const char* value);
} unclog_value_t;

typedef struct unclog_ini_s {
    const char* section;
    void (*handler)(void* user, const char* name, const char* value);
} unclog_ini_t;

#define MATCH(n, v) (strcmp(n, v) == 0)

static void unclog_defaults_handler(void* target, const char* name, const char* value) {
    unclog_global_t* g = target;
    if (MATCH(name, "Level")) g->defaults.level = unclog_level_tolevel(value);
}

static unclog_ini_t unclog_config[] = {
    {"Defaults", unclog_defaults_handler}, {NULL, NULL},
};

int unclog_ini_handler(void* user, const char* section, const char* name, const char* value) {
    unclog_ini_t* c = unclog_config;
    for (; c->section != NULL; c++) {
        if (strcmp(c->section, section) == 0) {
            c->handler(user, name, value);
            return 0;
        }
    }
    return 0;
}
