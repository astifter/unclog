#pragma once

#include <unclog/unclog.h>

#include <stdarg.h>
#include <stdint.h>

// initialize and configure, this is currently the same as unclog_config() but
// kept for furhter use. when unclog is initialized like this, unclog does not
// close by itself and has to be cleaned up by unclog_deinit()
void unclog_init(const char* config);
void unclog_config(const char* config);
void unclog_deinit(void);

// the following defines declare which details should be logged
#define UNCLOG_DETAILS_NONE 0x00
#define UNCLOG_DETAILS_TIMESTAMP 0x01
#define UNCLOG_DETAILS_LEVEL 0x02
#define UNCLOG_DETAILS_SOURCE 0x04
#define UNCLOG_DETAILS_FILE 0x08
#define UNCLOG_DETAILS_LINE 0x10
#define UNCLOG_DETAILS_MESSAGE 0x20
#define UNCLOG_DETAILS_FULL 0x3F

// have a key-value store for additional configurations for sinks
typedef struct unclog_config_value_s {
    char* name;
    char* value;
    struct unclog_config_value_s* next;
} unclog_config_value_t;
char* unclog_config_value_get(unclog_config_value_t* v, const char* name);

// the methods to be implemented by a logging sink
typedef void (*unclog_sink_init_t)(void** data, uint32_t details, unclog_config_value_t* config);
typedef void (*unclog_sink_log_t)(unclog_data_t* data, va_list list);
typedef void (*unclog_sink_deinit_t)(void* data);

// a data structure to collect the methods for a custom sink
typedef struct unclog_sink_methods_s {
    unclog_sink_init_t init;
    unclog_sink_log_t log;
    unclog_sink_deinit_t deinit;
} unclog_sink_methods_t;

// register a sink and (optinally) configure level and details. level can be
// UNCLOG_LEVEL_NONE and details can be UNCLOG_DETAILS_NONE to get the
// configured values
// a section with [sink.<name>] can be used to configure the sink in the config
void* unclog_sink_register_or_get(const char* name, int level, uint32_t details,
								  unclog_sink_methods_t methods);
