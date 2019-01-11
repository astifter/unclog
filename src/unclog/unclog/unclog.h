#pragma once

#define _POSIX_C_SOURCE 199309L

#include <limits.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNCLOG_LEVEL_MAXIMUM 0
#define UNCLOG_LEVEL_FATAL 200
#define UNCLOG_LEVEL_CRITICAL 400
#define UNCLOG_LEVEL_ERROR 600
#define UNCLOG_LEVEL_WARNING 800
#define UNCLOG_LEVEL_INFO 1000
#define UNCLOG_LEVEL_DEBUG 1200
#define UNCLOG_LEVEL_TRACE 1400
#define UNCLOG_LEVEL_MINIMUM INT_MAX

typedef struct unclog_s { int level; } unclog_t;

typedef struct unclog_data_s {
    unclog_t* ha;
    int le;
    const char* fi;
    const char* fu;
    unsigned int li;

    struct timespec no;
    struct unclog_sink_s* si;
} unclog_data_t;

unclog_t* unclog_open(const char* source);
void unclog_log(unclog_data_t data, ...);
void unclog_close(unclog_t* handle);

#ifndef UNCLOG_LEVEL_CUTOFF
#define UNCLOG_LEVEL_CUTOFF UNCLOG_LEVEL_MINIMUM
#endif

#define UNCLOG(h, l, ...)                                                                  \
    {                                                                                      \
        if (((l) <= (UNCLOG_LEVEL_CUTOFF)) && ((h) != NULL) && ((l) <= (h)->level))        \
            unclog_log(                                                                    \
                (unclog_data_t){                                                           \
                    .ha = (h), .le = (l), .fi = __FILE__, .fu = __func__, .li = __LINE__}, \
                ##__VA_ARGS__);                                                            \
    }

#define UL_FA(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_FATAL, ##__VA_ARGS__);
#define UL_CR(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_CRITICAL, ##__VA_ARGS__);
#define UL_ER(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_ERROR, ##__VA_ARGS__);
#define UL_WA(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_WARNING, ##__VA_ARGS__);
#define UL_IN(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_INFO, ##__VA_ARGS__);
#define UL_DE(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_DEBUG, ##__VA_ARGS__);
#define UL_TR(ha, ...) UNCLOG((ha), UNCLOG_LEVEL_TRACE, ##__VA_ARGS__);

#ifdef __cplusplus
}
#endif
