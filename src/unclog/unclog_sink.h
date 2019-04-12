#pragma once

#include <unclog/unclog_adv.h>

typedef struct unclog_sink_list_s {
    const char* name;
    unclog_sink_methods_t methods;
} unclog_sink_list_t;

extern unclog_sink_list_t unclog_sink_list[];

uint64_t unclog_sink_stderr_get_num_messages(void* data);
