#define _BSD_SOURCE

#include "unclog_int.h"

static pthread_mutex_t unclog_mutex = PTHREAD_MUTEX_INITIALIZER;
static unclog_global_t* unclog_global = NULL;

unclog_t* unclog_open(const char* source) {
  if (unclog_global == NULL) {
    pthread_mutex_lock(&unclog_mutex);
    if (unclog_global == NULL) {
      unclog_global = malloc(sizeof(unclog_global_t));
      unclog_init(unclog_global);
    }
    pthread_mutex_unlock(&unclog_mutex);
  }
  unclog_source_t* handle = malloc(sizeof(unclog_source_t));
  handle->source = strdup(source);
  return (unclog_t*)handle;
}

#define UNCLOG_HANDLE(ha) \
  if (ha == NULL) return; \
  unclog_sink_t* handle = (unclog_sink_t*)(ha);

void unclog_log(unclog_t* public_handle, unsigned int level, const char* file,
                const char* func, unsigned int line, const char* fmt, ...) {
  UNCLOG_HANDLE(public_handle);
}

void unclog_close(unclog_t* public_handle) { UNCLOG_HANDLE(public_handle); }
