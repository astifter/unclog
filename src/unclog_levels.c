#include "unclog_int.h"

#include <string.h>

typedef struct unclog_levels_s {
    int level;
    const char* name;
    char shortname;
} unclog_levels_t;

static unclog_levels_t unclog_levels[] = {
    {1400, "Fatal", 'F'}, {1200, "Critical", 'C'}, {1000, "Error", 'E'}, {800, "Warning", 'W'},
    {600, "Info", 'I'},   {400, "Debug", 'D'},     {200, "Trace", 'T'},  {-1, NULL, '\0'},
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
        if (level >= l->level) return l->shortname;
    }
    return '?';
}
