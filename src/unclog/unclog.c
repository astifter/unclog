#include <unclog/unclog_adv.h>

#include <ini.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unistd.h>

#define MATCH(s1, s2) (strcmp(s1, s2) == 0)

const char* unclog_config_locations[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

static void* malloc_and_clear(size_t size) {
    void* rv = malloc(size);
    memset(rv, 0, size);
    return rv;
}

struct unclog_levels_s {
    int level;
    const char* name;
} unclog_levels[] = {
    {UNCLOG_LEVEL_FATAL, "Fatal"}, {UNCLOG_LEVEL_CRITICAL, "Critical"},
    {UNCLOG_LEVEL_ERROR, "Error"}, {UNCLOG_LEVEL_WARNING, "Warning"},
    {UNCLOG_LEVEL_INFO, "Info"},   {UNCLOG_LEVEL_DEBUG, "Debug"},
    {UNCLOG_LEVEL_TRACE, "Trace"}, {-1, NULL},
};

int unclog_level(const char* name) {
    struct unclog_levels_s* ul = unclog_levels;
    for (; ul->name != NULL; ul++) {
        if (MATCH(ul->name, name)) return ul->level;
    }
    return -1;
}

struct unclog_details_s {
    uint32_t detail;
    const char* name;
} unclog_details[] = {
    {UNCLOG_DETAILS_TIMESTAMP, "Time"}, {UNCLOG_DETAILS_LEVEL, "Level"},
    {UNCLOG_DETAILS_SOURCE, "Source"},  {UNCLOG_DETAILS_FILE, "File"},
    {UNCLOG_DETAILS_LINE, "Line"},      {UNCLOG_DETAILS_MESSAGE, "Message"},
    {UNCLOG_DETAILS_FULL, "Full"},      {-1, NULL},
};

uint32_t unclog_detail(const char* name) {
    struct unclog_details_s* ud = unclog_details;
    for (; ud->name != NULL; ud++) {
        if (MATCH(ud->name, name)) return ud->detail;
    }
    return UNCLOG_DETAILS_NONE;
}

typedef struct unclog_config_s {
    unclog_values_t settings;
    char* name;
    int name_size;

    LIST_ENTRY(unclog_config_s) entries;
} unclog_config_t;

typedef struct unclog_source_s {
    int level;
    char* name;

    LIST_ENTRY(unclog_source_s) entries;
} unclog_source_t;

typedef struct unclog_global_s {
    LIST_HEAD(unclog_source_config_list, unclog_config_s) configs;
    LIST_HEAD(unclog_source_list, unclog_source_s) sources;
} unclog_global_t;

static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP;
static unclog_global_t* unclog_global = NULL;

static unclog_config_t* _unclog_config_get(const char* source, int exact_match) {
    int final_size = -1;
    unclog_config_t* final = NULL;

    unclog_config_t* c = unclog_global->configs.lh_first;
    for (; c != NULL; c = c->entries.le_next) {
        if (c->name_size > final_size && strncmp(source, c->name, c->name_size) == 0) {
            final_size = c->name_size;
            final = c;
        }
    }

    if (final != NULL && exact_match && (strcmp(final->name, source) != 0)) return NULL;
    return final;
}

static int _unclog_config_handler(void* user, const char* section, const char* name,
                                  const char* value) {
    (void)user;
    fprintf(stderr, "CONFIG: %s/%s/%s\n", section, name, value);

    unclog_config_t* config = NULL;
    if (MATCH(section, "defaults")) {
        config = _unclog_config_get("", 1);
    } else {
        config = _unclog_config_get(section, 1);
    }
    if (config == NULL) {
        config = malloc_and_clear(sizeof(unclog_config_t));
        config->settings.level = UNCLOG_LEVEL_DEFAULT;
        config->settings.details = UNCLOG_DETAILS_DEFAULT;
        config->name = strdup(section);
        config->name_size = strlen(config->name);

        LIST_INSERT_HEAD(&unclog_global->configs, config, entries);
    }

    if (MATCH(name, "Level")) {
        int level = unclog_level(value);
        if (level != -1) config->settings.level = level;
    } else if (MATCH(name, "Details")) {
        config->settings.details = 0;
        char* v = strdup(value);
        char* state;
        char* tok = strtok_r(v, ",", &state);
        while (tok != NULL) {
            config->settings.details |= unclog_detail(tok);
            tok = strtok_r(NULL, ",", &state);
        }
        free(v);
    }

    return 0;
}

static void _unclog_config(void) {
    const char** f = unclog_config_locations;
    for (; *f != NULL; f++) {
        if (access(*f, R_OK) != 0) continue;
        ini_parse(*f, _unclog_config_handler, NULL);
    }
}

static unclog_source_t* unclog_source_create_or_get(const char* source) {
    pthread_rwlock_wrlock(&unclog_mutex);
    unclog_source_t* s = unclog_global->sources.lh_first;
    for (; s != NULL; s = s->entries.le_next) {
        if (MATCH(s->name, source)) break;
    }
    if (s == NULL) {
        char configname[256] = {0};
        sprintf(configname, "source.%s", source);
        unclog_config_t* config = _unclog_config_get(configname, 0);

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

            unclog_config_t* c = malloc_and_clear(sizeof(unclog_config_t));
            c->settings.level = UNCLOG_LEVEL_DEFAULT;
            c->settings.details = UNCLOG_DETAILS_DEFAULT;
            c->name = strdup("");
            c->name_size = strlen(c->name);

            LIST_INSERT_HEAD(&unclog_global->configs, c, entries);

            _unclog_config();
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
    unclog_source_t* source = (unclog_source_t*)handle;

    pthread_rwlock_wrlock(&unclog_mutex);
    LIST_REMOVE(source, entries);
    free(source->name);
    free(source);

    if (unclog_global->sources.lh_first == NULL) {
        unclog_config_t* c = unclog_global->configs.lh_first;
        while (c != NULL) {
            unclog_config_t* d = c;
            c = c->entries.le_next;

            free(d->name);
            free(d);
        }

        free(unclog_global);
        unclog_global = NULL;
    }
    pthread_rwlock_unlock(&unclog_mutex);
}
