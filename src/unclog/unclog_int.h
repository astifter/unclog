#pragma once

#include "unclog/unclog_adv.h"

#include <linux/limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct unclog_source_s {
    int level;  // this data structure is cast from unclog_t so the first items has to be equal to
                // unclog_t
    int active;
    int initialized;

    char* source;
    struct unclog_source_s* next;
} unclog_source_t;

struct unclog_sink_internal_s {
    unclog_sink_methods_t methods;
    int registered;

    char* sink;
    struct unclog_sink_s* next;
};

typedef struct unclog_global_s {
    unclog_values_t defaults;
    int sinks_defined;
    int initialized;

    unclog_source_t* sources;
    unclog_sink_t* sinks;
} unclog_global_t;

extern const unclog_values_t unclog_defaults;

int unclog_ini_handler(void* data, const char* section, const char* name, const char* value);

unclog_global_t* unclog_global_create(const char* config, int usefile, int initialized);
void unclog_global_destroy(unclog_global_t* global);
void unclog_global_configure(unclog_global_t* global, const char* config, int usefile,
                             int initialized);

void unclog_global_source_add(unclog_global_t* global, unclog_source_t* source);
unclog_source_t* unclog_global_source_get(unclog_global_t* global, const char* source);
int unclog_global_source_remove(unclog_global_t* global, unclog_source_t* source);

void unclog_global_sink_add(unclog_global_t* global, unclog_sink_t* source);
unclog_sink_t* unclog_global_sink_get(unclog_global_t* global, const char* sink);
void unclog_global_sink_clear(unclog_global_t* global, int include_registered);

void unclog_global_dump_config(unclog_global_t* global);

unclog_source_t* unclog_source_create(int level, const char* name);
void unclog_source_destroy(unclog_source_t* source);

unclog_keyvalue_t* unclog_keyvalue_create(const char* key, const char* value);
void unclog_keyvalue_destroy(unclog_keyvalue_t* v);
char* unclog_keyvalue_get(unclog_keyvalue_t* v, const char* key);

unclog_sink_t* unclog_sink_create(unclog_values_t* settings, const char* name);
void unclog_sink_destroy(unclog_sink_t* sink);
void unclog_sink_add_keyvalue(unclog_sink_t* sink, const char* key, const char* value);

int unclog_level_tolevel(const char* value);
char unclog_level_tochar(int level);
const char* unclog_level_tostr(int level);

uint32_t unclog_details_todetail(const char* value);
char* unclog_details_tostr(uint32_t details);

// for testing
typedef struct unclog_levels_s {
    int level;
    const char* name;
    char shortname;
} unclog_levels_t;

typedef struct unclog_details_s {
    uint32_t detail;
    const char* name;
} unclog_details_t;

extern unclog_global_t* unclog_global;
extern unclog_levels_t unclog_levels[];
extern unclog_details_t unclog_details[];
