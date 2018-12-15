#pragma once

#include "unclog.h"

#include <stdint.h>

typedef struct unclog_source_s {
	unclog_t public;
    struct unclog_sink_s* sinks;

    char* source;
    struct unclog_source_s* next;
} unclog_source_t;

#define UNCLOG_OPT_TIMESTAMP 0x00000001
#define UNCLOG_OPT_SOURCE 0x00000002
#define UNCLOG_OPT_LOCATION 0x00000004
#define UNCLOG_OPT_FILE 0x00000008
#define UNCLOG_OPT_LINE 0x00000010
#define UNCLOG_OPT_MESSAGE 0x00000020
#define UNCLOG_OPT_DEFAULTS (UNCLOG_OPT_TIMESTAMP | UNCLOG_OPT_SOURCE | UNCLOG_OPT_MESSAGE)

typedef struct unclog_keyvalue_s {
	char* key;
	char* value;
	struct unclog_keyvalue_s* next;
} unclog_keyvalue_t;

typedef struct unclog_values_s {
    int level;
    uint32_t options;
} unclog_values_t;

typedef struct unclog_sink_s {
	unclog_values_t common;
	unclog_keyvalue_t* values;

    char* sink;
    struct unclog_sink_s* next;
} unclog_sink_t;

typedef struct unclog_global_s {
    unclog_values_t defaults;

    unclog_source_t* sources;
    unclog_sink_t* sinks;
} unclog_global_t;

#define UNCLOG_LEVEL_DEFAULT UNCLOG_LEVEL_WARNING

int unclog_ini_handler(void* user, const char* section, const char* name, const char* value);

unclog_global_t* unclog_global_create(void);
void unclog_global_destroy(unclog_global_t* global);
void unclog_global_source_add(unclog_global_t* global, unclog_source_t* source);
void unclog_global_source_remove(unclog_global_t* global, unclog_source_t* source);
void unclog_global_sink_add(unclog_global_t* global, unclog_sink_t* handle);
unclog_sink_t* unclog_global_sink_get(unclog_global_t* global, const char* section);

unclog_source_t* unclog_source_create(unclog_values_t* defaults, const char* source);
void unclog_source_destroy(unclog_source_t* source);

unclog_sink_t* unclog_sink_create(unclog_values_t* defaults, const char* sink);
void unclog_sink_destroy(unclog_sink_t* sink);
void unclog_sink_add_keyvalue(unclog_sink_t* sink, const char* key, const char* value); 

int unclog_level_tolevel(const char* value);
char unclog_level_tochar(int level);
char* unclog_level_tostr(int level);
uint32_t unclog_details_todetail(const char* value);
