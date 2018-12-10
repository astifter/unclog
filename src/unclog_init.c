#include "unclog_int.h"

typedef struct unclog_value_s {
    const char* name;
    int (*unclog_value_handler)(void* user, const char* value);
} unclog_value_t;

typedef struct unclog_ini_s {
    const char* section;
    unclog_value_t* values; 
} unclog_ini_t;

static const char* unclog_ini_files[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

static int unclog_level_handler(void* user, const char* value) {
    fprintf(stderr, "unclog_level_handler: %s\n", value);
    return 0;
}

static unclog_value_t unclog_config_defaults[] = {
  { "Level", unclog_level_handler },
  { NULL, NULL },
};

static unclog_ini_t unclog_config[] = {
  { "Defaults", unclog_config_defaults },
  { NULL, NULL },
};

static int unclog_ini_handler(void* user, const char* section, const char* name,
                              const char* value) {
    for(unclog_ini_t* c = unclog_config; c->section != NULL; c++) {
        if (strcmp(c->section, section) == 0) {
            for(unclog_value_t* v = c->values; v->name != NULL; v++) {
                if (strcmp(v->name, name) == 0) {
                    v->unclog_value_handler(user, value);
                }
            }
        }
    }
    return 0;
}

void unclog_init(unclog_global_t* g) {
    for(char** f = unclog_ini_files; *f != NULL; f++) {
        if(access(*f, R_OK) != 0) continue;
        ini_parse(*f, unclog_ini_handler, g);
    }
}

