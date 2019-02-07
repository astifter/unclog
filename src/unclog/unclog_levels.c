#include "unclog_int.h"

// clang-format off
unclog_levels_t unclog_levels[] = {
    {UNCLOG_LEVEL_FATAL, "Fatal", 'F'},
    {UNCLOG_LEVEL_CRITICAL, "Critical", 'C'},
    {UNCLOG_LEVEL_ERROR, "Error", 'E'},
    {UNCLOG_LEVEL_WARNING, "Warning", 'W'},
    {UNCLOG_LEVEL_INFO, "Info", 'I'},
    {UNCLOG_LEVEL_DEBUG, "Debug", 'D'},
    {UNCLOG_LEVEL_TRACE, "Trace", 'T'},
    {-1, NULL, '\0'},
};

unclog_details_t unclog_details[] = {
    {UNCLOG_OPT_TIMESTAMP, "Time"},
    {UNCLOG_OPT_LEVEL, "Level"},
    {UNCLOG_OPT_SOURCE, "Source"},
    {UNCLOG_OPT_FILE, "File"},
    {UNCLOG_OPT_LINE, "Line"},
    {UNCLOG_OPT_MESSAGE, "Message"},
    {UNCLOG_OPT_MAXIMUM, "Full"},
    {-1, NULL},
};
// clang-format on

int unclog_level_tolevel(const char* value) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (strcmp(l->name, value) == 0) {
            return l->level;
        }
    }
    return unclog_defaults.level;
}

char unclog_level_tochar(int level) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (UNCLOG_LEVEL_COMPARE(level, l->level)) {
            return l->shortname;
        }
    }
    return 'T';
}

const char* unclog_level_tostr(int level) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (UNCLOG_LEVEL_COMPARE(level, l->level)) {
            return l->name;
        }
    }
    return "Trace";
}

uint32_t unclog_details_todetail(const char* value) {
    for (unclog_details_t* d = unclog_details; d->name != NULL; d++) {
        if (strcmp(d->name, value) == 0) {
            return d->detail;
        }
    }
    return 0;
}

char* unclog_details_tostr(uint32_t details) {
    char* retval = malloc(4096);
    memset(retval, 0, 4096);
    char* buffer = retval;

    int next = 0;
    unclog_details_t* d = unclog_details;
    for (; d->name != NULL; d++) {
        if (d->detail == UNCLOG_OPT_MAXIMUM) continue;
        if (!(d->detail & details)) continue;

        if (next != 0) buffer += sprintf(buffer, ",");
        next = 1;

        buffer += sprintf(buffer, "%s", d->name);
    }
    return retval;
}
