#pragma once

#include "unclog.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <ini.h>
#include <unistd.h>

typedef struct unclog_source_s {
    unclog_t pub;
    const char* source;
    unclog_t* next;
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

void unclog_init(unclog_global_t* g);

typedef struct unclog_levels_s {
    int level;
    const char* name;
    char shortname;
} unclog_levels_t;

extern unclog_levels_t unclog_levels[];

#define UNCLOG_LEVEL_DEFAULT UNCLOG_LEVEL_WARNING
