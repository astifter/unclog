#define _GNU_SOURCE

#include "unclog_int.h"

const unclog_values_t unclog_defaults = {
    .level = UNCLOG_LEVEL_WARNING,
    .details = (UNCLOG_OPT_TIMESTAMP | UNCLOG_OPT_LEVEL | UNCLOG_OPT_SOURCE | UNCLOG_OPT_MESSAGE),
};

static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_INITIALIZER;
unclog_global_t* unclog_global = NULL;

static void unclog_init_nolock(const char* config, int usefile, int initialized) {
    if (unclog_global == NULL) {
        unclog_global = unclog_global_create(config, usefile, initialized);
    }
}

static void unclog_deinit_nolock(void) {
    if (unclog_global != NULL) {
        unclog_global_destroy(unclog_global);
        unclog_global = NULL;
    }
}

void unclog_init(const char* config) {
    pthread_rwlock_wrlock(&unclog_mutex);

    unclog_deinit_nolock();
    unclog_init_nolock(config, 0, 1);

    pthread_rwlock_unlock(&unclog_mutex);

    // unclog_global_dump_config(unclog_global);
}

void unclog_deinit(void) {
    pthread_rwlock_wrlock(&unclog_mutex);

    unclog_deinit_nolock();

    pthread_rwlock_unlock(&unclog_mutex);
}

void unclog_reinit(const char* config) {
    pthread_rwlock_wrlock(&unclog_mutex);

    // unclog_global_dump_config(unclog_global);
    unclog_global_sink_clear(unclog_global, 0);
    unclog_global_configure(unclog_global, config, 0, 1);
    // unclog_global_dump_config(unclog_global);

    pthread_rwlock_unlock(&unclog_mutex);
}

void unclog_sink_register(const char* name, unclog_values_t* settings,
                          unclog_sink_methods_t methods) {
    pthread_rwlock_wrlock(&unclog_mutex);

    unclog_sink_t* sink = unclog_global_sink_get(unclog_global, name);
    if (sink == NULL) {
        sink = unclog_sink_create(settings, name);
        unclog_global_sink_add(unclog_global, sink);
    }

    if (settings != NULL) {
        memcpy(&sink->settings, settings, sizeof(unclog_values_t));
        if (settings->level == 0) sink->settings.level = unclog_global->defaults.level;
        if (settings->details == 0) sink->settings.details = unclog_global->defaults.details;
    }

    sink->i->methods = methods;
    sink->i->registered = 1;

    if (sink->i->methods.init != NULL) sink->i->methods.init(sink);

    pthread_rwlock_unlock(&unclog_mutex);

    // unclog_global_dump_config(unclog_global);
}

unclog_t* unclog_open(const char* source) {
    pthread_rwlock_wrlock(&unclog_mutex);

    unclog_init_nolock(NULL, 1, 0);

    unclog_source_t* handle = unclog_global_source_get(unclog_global, source);
    if (handle == NULL) {
        handle = unclog_source_create(unclog_global->defaults.level, source);
        unclog_global_source_add(unclog_global, handle);
    }
    handle->active++;

    pthread_rwlock_unlock(&unclog_mutex);

    // unclog_global_dump_config(unclog_global);
    return (unclog_t*)handle;
}

void unclog_log(unclog_data_t data, ...) {
    if (data.ha == NULL) return;

    if (data.le > data.ha->level) return;

    clock_gettime(CLOCK_REALTIME, &data.no);

    pthread_rwlock_rdlock(&unclog_mutex);
    unclog_sink_t* sink = unclog_global->sinks;
    for (; sink != NULL; sink = sink->i->next) {
        if (sink->i->methods.log == NULL) continue;
        if (data.le > sink->settings.level) continue;

        va_list al;
        va_start(al, data);
        data.si = sink;
        sink->i->methods.log(&data, al);
        va_end(al);
    }
    pthread_rwlock_unlock(&unclog_mutex);
}

void unclog_close(unclog_t* public_handle) {
    if (public_handle == NULL) return;
    unclog_source_t* handle = (unclog_source_t*)public_handle;

    pthread_rwlock_wrlock(&unclog_mutex);

    handle->active--;
    int has_active_handles;
    if (handle->active == 0 && handle->initialized == 0) {
        has_active_handles = unclog_global_source_remove(unclog_global, handle);
    } else {
        has_active_handles = 1;
    }

    if (!has_active_handles && unclog_global->initialized != 1) {
        unclog_deinit_nolock();
    }

    pthread_rwlock_unlock(&unclog_mutex);
}
