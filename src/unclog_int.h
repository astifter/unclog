#pragma once

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <ini.h>

typedef struct unclog_s {
    const char* source;
} unclog_t;

typedef struct unclog_global_s {
    size_t handles;
} unclog_global_t;

void unclog_init(unclog_global_t* g);
