#pragma once

#include "unclog.h"

typedef struct unclog_source_s {
    int level; /* this is the same as in unclog_t, make sure that unclog_t is updated as well when
                * this has to be changed. */
    char* source;
    struct unclog_source_s* next;
} unclog_source_t;

typedef struct unclog_sink_s {
    char* sink;
    struct unclog_sink_s* next;
} unclog_sink_t;

typedef struct unclog_defaults_s { int level; } unclog_defaults_t;

typedef struct unclog_global_s {
    unclog_defaults_t defaults;
    unclog_source_t* sources;
    unclog_sink_t* sinks;
} unclog_global_t;

#define UNCLOG_LEVEL_DEFAULT UNCLOG_LEVEL_WARNING

int unclog_ini_handler(void* user, const char* section, const char* name, const char* value);

unclog_global_t* unclog_global_create(void);
void unclog_global_destroy(unclog_global_t* global);
void unclog_global_source_add(unclog_global_t* global, unclog_source_t* source);
void unclog_global_source_remove(unclog_global_t* global, unclog_source_t* source);

unclog_source_t* unclog_source_create(unclog_defaults_t* defaults, const char* source);
void unclog_source_destroy(unclog_source_t* source);

int unclog_level_tolevel(const char* value);
char unclog_level_tochar(int level);
