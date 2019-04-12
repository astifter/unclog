#pragma once

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#define _GNU_SOURCE

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// define logging levels, additionally add _NONE as undefined value and the
// comparator function
#define UNCLOG_LEVEL_NONE -1
#define UNCLOG_LEVEL_FATAL 1
#define UNCLOG_LEVEL_CRITICAL 2
#define UNCLOG_LEVEL_ERROR 3
#define UNCLOG_LEVEL_WARNING 4
#define UNCLOG_LEVEL_INFO 5
#define UNCLOG_LEVEL_DEBUG 6
#define UNCLOG_LEVEL_TRACE 7
#define UNCLOG_LEVEL_COMPARE(l, m) ((l) <= (m))

// declare the defauls, when the defaults are set during compile those values
// are used troughout the library
#ifndef UNCLOG_LEVEL_DEFAULT
#define UNCLOG_LEVEL_DEFAULT UNCLOG_LEVEL_WARNING
#endif
#ifndef UNCLOG_DETAILS_DEFAULT
#define UNCLOG_DETAILS_DEFAULT                                                 \
    (UNCLOG_DETAILS_TIMESTAMP | UNCLOG_DETAILS_LEVEL | UNCLOG_DETAILS_SOURCE | \
     UNCLOG_DETAILS_MESSAGE)
#endif

// the semi-transpared logging handle, level is accessible from outside so the
// logging defines can make a decision about logging without acually doing the
// call
typedef struct unclog_s {
    int level;  // this level is the same as in unclog_source_t
} unclog_t;

// a data structure that is filled during a logging call, this can then be
// handed to the logging sinks without
// - having to create a variable during the call
// - having to copy the data to the sinks
// the time in ti and the data in da is filled during the logging call by the
// library
typedef struct unclog_data_s {
    unclog_t* ha;
    int le;
    const char* fi;
    const char* fu;
    unsigned int li;

    struct tm ti;
    void* da;
} unclog_data_t;

// the open, log and close methods:
// - initializes unclog if not done and open creates a new logging source with
//   the given name
// - log does the actual logging
// - close shuts down the logging source and (if this was the last) clean up
//   unclog
unclog_t* unclog_open(const char* source);
void unclog_log(unclog_data_t data, ...);
void unclog_close(unclog_t* handle);

// when the cutoff is defined the logging calls are stripped above a certain
// level so that not even further comparisons are done
#ifndef UNCLOG_LEVEL_CUTOFF
#define UNCLOG_LEVEL_CUTOFF UNCLOG_LEVEL_TRACE
#endif

// the universal logging call:
// - first compare the cutoff, since this is a const comparison the if is
//   stripped when the cutoff is above the log call level
// - then make sure the handle is valid and compare the handle's level with the
//   calls level
// - when all comparisons check out, fill a data structure with relevant data
//   and do the logging call
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

// convenience handles for doing logging at certain levels, all of those have 2
// letter abbreviations to keep the code lean and tidy
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
