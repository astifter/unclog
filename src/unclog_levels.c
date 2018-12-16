#include "unclog_int.h"

#include <string.h>

typedef struct unclog_levels_s {
    int level;
    const char* name;
    char shortname;
} unclog_levels_t;

static unclog_levels_t unclog_levels[] = {
    {UNCLOG_LEVEL_FATAL, "Fatal", 'F'}, {UNCLOG_LEVEL_CRITICAL, "Critical", 'C'},
    {UNCLOG_LEVEL_ERROR, "Error", 'E'}, {UNCLOG_LEVEL_WARNING, "Warning", 'W'},
    {UNCLOG_LEVEL_INFO, "Info", 'I'},   {UNCLOG_LEVEL_DEBUG, "Debug", 'D'},
    {UNCLOG_LEVEL_TRACE, "Trace", 'T'}, {0, NULL, '\0'},
};

typedef struct unclog_details_s {
    uint32_t option;
    const char* name;
} unclog_details_t;

static unclog_details_t unclog_details[] = {
    {UNCLOG_OPT_LEVEL, "Level"},     {UNCLOG_OPT_TIMESTAMP, "Timestamp"},
    {UNCLOG_OPT_SOURCE, "Source"},   {UNCLOG_OPT_LOCATION, "Location"},
    {UNCLOG_OPT_FILE, "File"},       {UNCLOG_OPT_LINE, "Line"},
    {UNCLOG_OPT_MESSAGE, "Message"}, {-1, NULL},
};

int unclog_level_tolevel(const char* value) {
    unclog_levels_t* l = unclog_levels;
    for (; l->name != NULL; l++) {
        if (strcmp(l->name, value) == 0) {
            return l->level;
        }
    }
    return UNCLOG_LEVEL_DEFAULT;
}

char unclog_level_tochar(int level) {
    unclog_levels_t* l = unclog_levels;
    for (; l->name != NULL; l++) {
        if (level <= l->level) return l->shortname;
    }
    return 'M';
}

const char* unclog_level_tostr(int level) {
    unclog_levels_t* l = unclog_levels;
    for (; l->name != NULL; l++) {
        if (level <= l->level) return l->name;
    }
    return "Minimum";
}

uint32_t unclog_details_todetail(const char* value) {
    unclog_details_t* d = unclog_details;
    for (; d->name != NULL; d++) {
        if (strcmp(d->name, value) == 0) {
            return d->option;
        }
    }
    return 0;
}
