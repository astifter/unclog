#include <unclog/unclog_adv.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

static void* malloc_and_clear(size_t size) {
    void* rv = malloc(size);
    memset(rv, 0, size);
    return rv;
}

typedef struct unclog_source_config_s {
    unclog_values_t settings;
    char* name;
    int name_size;

    LIST_ENTRY(unclog_source_config_s) entries;
} unclog_source_config_t;

typedef struct unclog_source_s {
    int level;
    char* name;

    LIST_ENTRY(unclog_source_s) entries;
} unclog_source_t;

typedef struct unclog_global_s {
    LIST_HEAD(unclog_source_config_list, unclog_source_config_s) configs;
    LIST_HEAD(unclog_source_list, unclog_source_s) sources;
} unclog_global_t;

static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP;
static unclog_global_t* unclog_global = NULL;

static unclog_source_config_t* _unclog_source_config_get(const char* source) {
    int final_size = -1;
    unclog_source_config_t* final = NULL;

    unclog_source_config_t* c = unclog_global->configs.lh_first;
    while (c != NULL) {
        if (c->name_size > final_size) {
            if (strncmp(source, c->name, c->name_size) == 0) {
                final_size = c->name_size;
                final = c;
            }
        }
        c = c->entries.le_next;
    }

    return final;
}

static unclog_source_t* unclog_source_create_or_get(const char* source) {
    pthread_rwlock_wrlock(&unclog_mutex);
    unclog_source_t* s = unclog_global->sources.lh_first;
    while (s != NULL) {
        if (strcmp(s->name, source) == 0) break;
        s = s->entries.le_next;
    }
    if (s == NULL) {
        unclog_source_config_t* config = _unclog_source_config_get(source);

        s = malloc_and_clear(sizeof(unclog_source_t));
        s->name = strdup(source);
        s->level = config->settings.level;

        LIST_INSERT_HEAD(&unclog_global->sources, s, entries);
    }
    pthread_rwlock_unlock(&unclog_mutex);
    return s;
}

unclog_t* unclog_open(const char* source) {
    if (unclog_global == NULL) {
        pthread_rwlock_wrlock(&unclog_mutex);
        if (unclog_global == NULL) {
            unclog_global = malloc_and_clear(sizeof(unclog_global_t));
            LIST_INIT(&unclog_global->configs);
            LIST_INIT(&unclog_global->sources);

            unclog_source_config_t* c = malloc_and_clear(sizeof(unclog_source_config_t));
            c->settings.level = UNCLOG_LEVEL_DEFAULT;
            c->settings.details = UNCLOG_DETAILS_DEFAULT;
            c->name = strdup("");
            c->name_size = strlen(c->name);

            LIST_INSERT_HEAD(&unclog_global->configs, c, entries);
        }
        pthread_rwlock_unlock(&unclog_mutex);
    }

    unclog_source_t* h = unclog_source_create_or_get(source);
    return (unclog_t*)h;
}

void unclog_log(unclog_data_t data, ...) {
    pthread_rwlock_rdlock(&unclog_mutex);
    va_list list;
    va_start(list, data);
    const char* fmt = va_arg(list, const char*);
    vfprintf(stderr, fmt, list);
    fprintf(stderr, "\n");
    va_end(list);
    pthread_rwlock_unlock(&unclog_mutex);
}

void unclog_close(unclog_t* handle) {
    unclog_source_t* source = handle;
    pthread_rwlock_wrlock(&unclog_mutex);
    LIST_REMOVE(source, entries);
    free(source->name);
    free(source);

    if (unclog_global->sources.lh_first == NULL) {
        unclog_source_config_t* c = unclog_global->configs.lh_first;
        while (c != NULL) {
            unclog_source_config_t* d = c;
            c = c->entries.le_next;

            free(d->name);
            free(d);
        }
        free(unclog_global);
        unclog_global = NULL;
    }
    pthread_rwlock_unlock(&unclog_mutex);
}
