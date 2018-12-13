#include "unclog_int.h"

#include <stdlib.h>
#include <unistd.h>

#include <ini.h>

static const char* unclog_ini_files[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

unclog_global_t* unclog_global_create(void) {
	unclog_global_t* g = malloc(sizeof(unclog_global_t));

    for (const char** f = unclog_ini_files; *f != NULL; f++) {
        if (access(*f, R_OK) != 0) continue;
        ini_parse(*f, unclog_ini_handler, g);
        fprintf(stderr, "Defaults: Level: %d\n", g->level);
    }

	return g;
}

void unclog_global_destroy(unclog_global_t* global) {
	free(global);
}
