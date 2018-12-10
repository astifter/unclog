#include "unclog_int.h"

typedef struct unclog_value_s {
    const char* name;
    void (*unclog_value_handler)(void* user, const char* value);
} unclog_value_t;

typedef struct unclog_ini_s {
    const char* section;
    unclog_value_t* values; 
} unclog_ini_t;

unclog_levels_t unclog_levels[] = {
  { 1400, "Fatal",    'F' },
  { 1200, "Critical", 'C' },
  { 1000, "Error",    'E' },
  {  800, "Warning",  'W' },
  {  600, "Info",     'I' },
  {  400, "Debug",    'D' },
  {  200, "Trace",    'T' },
  {   -1, NULL,      '\0' },
};

static const char* unclog_ini_files[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

static void unclog_level_handler(void* target, const char* value) {
    for(unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (strcmp(l->name, value) == 0) {
            *(unsigned int*)target = l->level;
            return;
        }
    }
    *(unsigned int*)target = UNCLOG_LEVEL_DEFAULT;
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
    unclog_global_t* g = user;
    for(unclog_ini_t* c = unclog_config; c->section != NULL; c++) {
        if (strcmp(c->section, section) == 0) {
            for(unclog_value_t* v = c->values; v->name != NULL; v++) {
                if (strcmp(v->name, name) == 0) {
                    v->unclog_value_handler(&g->level, value);
                }
            }
        }
    }
    return 0;
}

void unclog_init(unclog_global_t* g) {
    for(const char** f = unclog_ini_files; *f != NULL; f++) {
        if(access(*f, R_OK) != 0) continue;
        ini_parse(*f, unclog_ini_handler, g);
        fprintf(stderr, "Defaults: Level: %d\n", g->level);
    }
}

