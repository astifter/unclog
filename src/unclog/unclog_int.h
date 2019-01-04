#pragma once

#include "unclog/unclog_adv.h"

#include <stdarg.h>
#include <stdint.h>
#include <time.h>

typedef struct unclog_source_s {
    int level;  // this data structure is cast from unclog_t so the first item has to be an int
                // level;
    int active;

    char* source;
    struct unclog_source_s* next;
} unclog_source_t;

#define UNCLOG_OPT_TIMESTAMP 0x01
#define UNCLOG_OPT_LEVEL 0x02
#define UNCLOG_OPT_SOURCE 0x04
#define UNCLOG_OPT_FILE 0x08
#define UNCLOG_OPT_LINE 0x10
#define UNCLOG_OPT_MESSAGE 0x20
#define UNCLOG_OPT_MAXIMUM 0x3F

typedef struct unclog_keyvalue_s {
    char* key;
    char* value;
    struct unclog_keyvalue_s* next;
} unclog_keyvalue_t;

typedef struct unclog_sink_s unclog_sink_t;

struct unclog_sink_s {
    unclog_values_t settings;
    unclog_keyvalue_t* values;
    unclog_sink_log_t log;

    char* sink;
    struct unclog_sink_s* next;
};

typedef struct unclog_global_s {
    unclog_values_t defaults;
    int sinks_defined;

    unclog_source_t* sources;
    unclog_sink_t* sinks;
} unclog_global_t;

extern const unclog_values_t unclog_defaults;

int unclog_ini_handler(void* data, const char* section, const char* name, const char* value);

unclog_global_t* unclog_global_create(const char* config, int usefile);
void* unclog_global_destroy(unclog_global_t* global);

void unclog_global_source_add(unclog_global_t* global, unclog_source_t* source);
unclog_source_t* unclog_global_source_get(unclog_global_t* global, const char* source);
int unclog_global_source_remove(unclog_global_t* global, unclog_source_t* source);

void unclog_global_sink_add(unclog_global_t* global, unclog_sink_t* source);
unclog_sink_t* unclog_global_sink_get(unclog_global_t* global, const char* sink);

void unclog_global_dump_config(unclog_global_t* global);

unclog_source_t* unclog_source_create(int level, const char* name);
void unclog_source_destroy(unclog_source_t* source);

unclog_sink_t* unclog_sink_create(unclog_values_t* settings, const char* name);
void unclog_sink_destroy(unclog_sink_t* sink);
void unclog_sink_add_keyvalue(unclog_sink_t* sink, const char* key, const char* value);

typedef struct unclog_levels_s {
    int level;
    const char* name;
    char shortname;
} unclog_levels_t;

int unclog_level_tolevel(const char* value);
char unclog_level_tochar(int level);
const char* unclog_level_tostr(int level);

typedef struct unclog_details_s {
    uint32_t detail;
    const char* name;
} unclog_details_t;

uint32_t unclog_details_todetail(const char* value);
char* unclog_details_tostr(uint32_t details);

// for testing
extern unclog_global_t* unclog_global;
extern unclog_levels_t unclog_levels[];
extern unclog_details_t unclog_details[];
