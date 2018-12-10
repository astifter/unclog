#pragma once

#include "unclog.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ini.h>

typedef struct unclog_s {
    const char* source;
} unclog_t;

typedef struct unclog_global_s {
    unsigned int level;
    size_t handles;
} unclog_global_t;

void unclog_init(unclog_global_t* g);

typedef struct unclog_levels_s {
    int level;
    const char* name;
    char shortname;
} unclog_levels_t;

extern unclog_levels_t unclog_levels[];

#define UNCLOG_LEVEL_DEFAULT UNCLOG_LEVEL_WARNING
