#pragma once

#include <unclog/unclog.h>

#include <stdarg.h>
#include <stdint.h>

void unclog_init(const char* config);
void unclog_reinit(const char* config);
void unclog_deinit(void);

#define UNCLOG_DETAILS_NONE 0x00
#define UNCLOG_DETAILS_TIMESTAMP 0x01
#define UNCLOG_DETAILS_LEVEL 0x02
#define UNCLOG_DETAILS_SOURCE 0x04
#define UNCLOG_DETAILS_FILE 0x08
#define UNCLOG_DETAILS_LINE 0x10
#define UNCLOG_DETAILS_MESSAGE 0x20
#define UNCLOG_DETAILS_FULL 0x3F

typedef struct unclog_values_s {
    int level;
    uint32_t details;
} unclog_values_t;

typedef struct unclog_keyvalue_s {
    char* key;
    char* value;
    struct unclog_keyvalue_s* next;
} unclog_keyvalue_t;

typedef struct unclog_sink_internal_s unclog_sink_internal_t;

typedef struct unclog_sink_s {
    unclog_values_t* settings;
    unclog_keyvalue_t* values;
    void* data;
    unclog_sink_internal_t* i;
} unclog_sink_t;

typedef void (*unclog_sink_init_t)(unclog_sink_t* sink);
typedef void (*unclog_sink_log_t)(unclog_data_t* d, va_list l);
typedef void (*unclog_sink_deinit_t)(unclog_sink_t* sink);

typedef struct unclog_sink_methods_s {
    unclog_sink_init_t init;
    unclog_sink_log_t log;
    unclog_sink_deinit_t deinit;
} unclog_sink_methods_t;

void unclog_sink_register(const char* name, unclog_values_t* settings,
                          unclog_sink_methods_t methods);
