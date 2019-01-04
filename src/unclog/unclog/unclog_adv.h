#pragma once

#include <unclog/unclog.h>

#include <stdarg.h>
#include <stdint.h>

void unclog_init(const char* config);
void unclog_deinit(void);

typedef void (*unclog_sink_log_t)(unclog_data_t* d, va_list l);

typedef struct unclog_values_s {
    int level;
    uint32_t details;
} unclog_values_t;

void unclog_sink_register(const char* name, unclog_values_t* settings, unclog_sink_log_t sink_cb);
