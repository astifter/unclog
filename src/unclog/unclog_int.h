#pragma once

#include <sys/queue.h>
#include <unclog/unclog_adv.h>

// logging sink contains settings for sink
typedef struct unclog_sink_s {
    int level;
    uint32_t details;
    char* name;
    struct unclog_sink_methods_s methods;
    void* internal;

    uint32_t flags;

    LIST_ENTRY(unclog_sink_s) entries;
} unclog_sink_t;

unclog_sink_t* _unclog_sink_get(const char* name);

// logging source only needs a logging level and a name for finding its
// configuration
typedef struct unclog_source_s {
    int level;  // this level is the same as in unclog_t
    char* name;

    uint32_t refcnt;

    LIST_ENTRY(unclog_source_s) entries;
} unclog_source_t;

unclog_source_t* _unclog_source_create_or_get(const char* source, int create);

// the global configuration structure holds lists of configs, sources and sinks
typedef struct unclog_global_s {
    uint32_t flags;
    LIST_HEAD(, unclog_config_s) configs;
    LIST_HEAD(, unclog_source_s) sources;
    LIST_HEAD(, unclog_sink_s) sinks;
} unclog_global_t;

extern unclog_global_t* unclog_global;

// together with the next function, parse a logging level
typedef struct unclog_levels_s {
    int level;
    const char* name;
} unclog_levels_t;

extern unclog_levels_t unclog_levels[];

int unclog_level(const char* name);

// together with the next function, parse a bunch of details
typedef struct unclog_details_s {
    uint32_t detail;
    const char* name;
} unclog_details_t;

extern unclog_details_t unclog_details[];

uint32_t unclog_detail(const char* name);
char* unclog_details_tostr(uint32_t details);

// config holds a bunch of values from a configuration section, details are
// only used for sinks
typedef struct unclog_config_s {
    int level;
    uint32_t details;
    char* name;
    int name_size;
    unclog_config_value_t* config;

    LIST_ENTRY(unclog_config_s) entries;
} unclog_config_t;

unclog_config_t* _unclog_config_get(const char* prefix, const char* source,
                                    int exact_match);
