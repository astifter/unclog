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

void unclog_level_handler(void* target, const char* value) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (strcmp(l->name, value) == 0) {
            *(unsigned int*)target = l->level;
            return;
        }
    }
    *(unsigned int*)target = UNCLOG_LEVEL_DEFAULT;
}

char unclog_level_tochar(int level) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
		if (level >= l->level)
			return l->shortname;
	}
	return '?';
}
