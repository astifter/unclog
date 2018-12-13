#pragma once

#include "unclog.h"

typedef struct unclog_source_s {
    unclog_t pub;
    char* source;
    struct unclog_source_s* next;
} unclog_source_t;

typedef struct unclog_sink_s {
    const char* sink;
    struct unclog_sink_s* next;
} unclog_sink_t;

typedef struct unclog_global_s {
    int level;
    unclog_source_t* sources;
    unclog_sink_t* sinks;
} unclog_global_t;

#define UNCLOG_LEVEL_DEFAULT UNCLOG_LEVEL_WARNING

int unclog_ini_handler(void* user, const char* section, const char* name, const char* value);

unclog_global_t* unclog_global_create(void);
void unclog_global_destroy(unclog_global_t* global);

unclog_source_t* unclog_source_create(const char* source);
void unclog_source_destroy(unclog_source_t* source);

void unclog_level_handler(void* target, const char* value);
char unclog_level_tochar(int level);
