#pragma once

#include <limits.h>
#include <stddef.h>

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

unclog_t* unclog_open(const char* source);
void unclog_log(unclog_t* handle, unsigned int level, const char* file, const char* func,
                unsigned int line, const char* fmt, ...);
void unclog_close(unclog_t* handle);

#ifndef UNCLOG_LEVEL_CUTOFF
#define UNCLOG_LEVEL_CUTOFF UNCLOG_LEVEL_MINIMUM
#endif

#define UNCLOG(ha, le, fi, fu, li, ...)                                  \
    {                                                                    \
        if ((le) <= (UNCLOG_LEVEL_CUTOFF))                               \
            if (((ha) != NULL) && ((le) <= (ha)->level))                 \
                unclog_log((ha), (le), (fi), (fu), (li), ##__VA_ARGS__); \
    }

#define UL_FAT(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_FATAL, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define UL_CRI(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_CRITICAL, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define UL_ERR(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define UL_WAR(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_WARNING, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define UL_INF(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_INFO, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define UL_DEB(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define UL_TRA(ha, ...) \
    UNCLOG((ha), UNCLOG_LEVEL_TRACE, __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#ifdef __cplusplus
}
#endif
