#pragma once

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include <limits.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNCLOG_LEVEL_FATAL 1
#define UNCLOG_LEVEL_CRITICAL 2
#define UNCLOG_LEVEL_ERROR 3
#define UNCLOG_LEVEL_WARNING 4
#define UNCLOG_LEVEL_INFO 5
#define UNCLOG_LEVEL_DEBUG 6
#define UNCLOG_LEVEL_TRACE 7
#define UNCLOG_LEVEL_COMPARE(l, m) ((l) <= (m))

typedef struct unclog_s { int level; } unclog_t;

typedef struct unclog_data_s {
    unclog_t* ha;
    int le;
    const char* fi;
    const char* fu;
    unsigned int li;

    struct timespec now;

    struct unclog_sink_s* sink;
} unclog_data_t;

unclog_t* unclog_open(const char* source);
void unclog_log(unclog_data_t data, ...);
void unclog_close(unclog_t* handle);

#ifndef UNCLOG_LEVEL_CUTOFF
#define UNCLOG_LEVEL_CUTOFF UNCLOG_LEVEL_TRACE
#endif

#define UNCLOG(h, l, ...)                                                                  \
    {                                                                                      \
        if (UNCLOG_LEVEL_COMPARE(l, UNCLOG_LEVEL_CUTOFF) && ((h) != NULL) &&               \
            UNCLOG_LEVEL_COMPARE(l, (h)->level)) {                                         \
            unclog_log(                                                                    \
                (unclog_data_t){                                                           \
                    .ha = (h), .le = (l), .fi = __FILE__, .fu = __func__, .li = __LINE__}, \
                ##__VA_ARGS__);                                                            \
        }                                                                                  \
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
