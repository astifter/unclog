#include <unclog/unclog_adv.h>

#include <ini.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unistd.h>

// have a define for strcmp to prevent code duplication
#define MATCH(s1, s2) (strcmp(s1, s2) == 0)

// list if locations for unclog.ini files
static const char* unclog_config_locations[] = {
    "./unclog.ini", "/etc/unclog.ini", NULL,
};

// helper for acquiring and clearing memory
static void* malloc_and_clear(size_t size) {
    void* rv = malloc(size);
    memset(rv, 0, size);
    return rv;
}

static struct unclog_levels_s {
    int level;
    const char* name;
} unclog_levels[] = {
    {UNCLOG_LEVEL_FATAL, "Fatal"}, {UNCLOG_LEVEL_CRITICAL, "Critical"},
    {UNCLOG_LEVEL_ERROR, "Error"}, {UNCLOG_LEVEL_WARNING, "Warning"},
    {UNCLOG_LEVEL_INFO, "Info"},   {UNCLOG_LEVEL_DEBUG, "Debug"},
    {UNCLOG_LEVEL_TRACE, "Trace"}, {-1, NULL},
};

static int unclog_level(const char* name) {
    struct unclog_levels_s* ul = unclog_levels;
    for (; ul->name != NULL; ul++) {
        if (MATCH(ul->name, name)) return ul->level;
    }
    return -1;
}

static struct unclog_details_s {
    uint32_t detail;
    const char* name;
} unclog_details[] = {
    {UNCLOG_DETAILS_TIMESTAMP, "Time"}, {UNCLOG_DETAILS_LEVEL, "Level"},
    {UNCLOG_DETAILS_SOURCE, "Source"},  {UNCLOG_DETAILS_FILE, "File"},
    {UNCLOG_DETAILS_LINE, "Line"},      {UNCLOG_DETAILS_MESSAGE, "Message"},
    {UNCLOG_DETAILS_FULL, "Full"},      {-1, NULL},
};

static uint32_t unclog_detail(const char* name) {
    struct unclog_details_s* ud = unclog_details;
    for (; ud->name != NULL; ud++) {
        if (MATCH(ud->name, name)) return ud->detail;
    }
    return UNCLOG_DETAILS_NONE;
}

// create a configuration key-value pair
static unclog_config_value_t* unclog_config_value_create(const char* name, const char* value) {
    unclog_config_value_t* v = malloc_and_clear(sizeof(unclog_config_value_t));
    v->name = strdup(name);
    v->value = strdup(value);
    return v;
}

// config holds a bunch of values from a configuration section, details are
// only used for sinks.
typedef struct unclog_config_s {
    int level;
    uint32_t details;
    char* name;
    int name_size;
    unclog_config_value_t* config;

    LIST_ENTRY(unclog_config_s) entries;
} unclog_config_t;

// logging source only needs a logging level and a name for finding its
// configuration
typedef struct unclog_source_s {
    int level;  // this level is the same as in unclog_t
    char* name;

    LIST_ENTRY(unclog_source_s) entries;
} unclog_source_t;

// logging sink contains settings for sink
typedef struct unclog_sink_s {
    int level;
    uint32_t details;
    char* name;
    struct unclog_sink_methods_s methods;
    void* internal;

    LIST_ENTRY(unclog_sink_s) entries;
} unclog_sink_t;

// the global configuration structure holds lists of configs, sources and sinks
typedef struct unclog_global_s {
    LIST_HEAD(, unclog_config_s) configs;
    LIST_HEAD(, unclog_source_s) sources;
    LIST_HEAD(, unclog_sink_s) sinks;
} unclog_global_t;

// global configuration and mutex for mutual access
static pthread_rwlock_t unclog_mutex = PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP;
static unclog_global_t* unclog_global = NULL;

// create a configuration with given level and details, but still no config key-values
static unclog_config_t* _unclog_config_create(const char* name, int level, uint32_t details) {
    unclog_config_t* c = malloc_and_clear(sizeof(unclog_config_t));
    c->level = level;
    c->details = details;
    c->name = strdup(name);
    c->name_size = strlen(c->name);
    return c;
}

// search for a configuration, either get an exact match for the name (for
// adding to the config) or a longest match to get tiered config values.
// also there is a config with empty name ("") that serves for storing the
// defaults
static unclog_config_t* _unclog_config_get(const char* prefix, const char* source,
                                           int exact_match) {
    char* configname;
    if (prefix != NULL)
        asprintf(&configname, "%s.%s", prefix, source);
    else
        asprintf(&configname, "%s", source);

    int final_size = -1;
    unclog_config_t* final = NULL;

    unclog_config_t* c = unclog_global->configs.lh_first;
    for (; c != NULL; c = c->entries.le_next) {
        if (c->name_size > final_size && strncmp(configname, c->name, c->name_size) == 0) {
            final_size = c->name_size;
            final = c;
        }
    }

    // when an exact_match is required make sure the found item matches
    if (exact_match && final != NULL && !MATCH(final->name, configname)) final = NULL;

    free(configname);
    return final;
}

// this is the callback for inih which is called for each configuration line:
// - first fetch the relevant configuration by exact match
// - if no config was found already, create one with default values
// - the parse configuration value
static int _unclog_config_handler(void* user, const char* section, const char* name,
                                  const char* value) {
    (void)user;
    fprintf(stderr, "CONFIG: %s: %s=%s\n", section, name, value);

    const char* configname = "";
    if (!MATCH(section, "defaults")) {
        configname = section;
    }
    unclog_config_t* c = _unclog_config_get(NULL, configname, 1);
    if (c == NULL) {
        c = _unclog_config_create(configname, UNCLOG_LEVEL_DEFAULT, UNCLOG_DETAILS_DEFAULT);
        LIST_INSERT_HEAD(&unclog_global->configs, c, entries);
    }

    if (MATCH(name, "Level")) {
        int level = unclog_level(value);
        if (level != -1) c->level = level;
    } else if (MATCH(name, "Details")) {
        c->details = 0;
        char* v = strdup(value);
        char* state = NULL;
        char* tok = strtok_r(v, ",", &state);
        while (tok != NULL) {
            c->details |= unclog_detail(tok);
            tok = strtok_r(NULL, ",", &state);
        }
        free(v);
    } else {
        unclog_config_value_t* v = unclog_config_value_create(name, value);
        v->next = c->config;
        c->config = v;
    }

    return 0;
}

// remove all configuration values and clean up memory
static void _unclog_config_clear(void) {
    unclog_config_t* c = unclog_global->configs.lh_first;
    while (c != NULL) {
        LIST_REMOVE(c, entries);

        while (c->config != NULL) {
            unclog_config_value_t* v = c->config;
            c->config = c->config->next;
            free(v->name);
            free(v->value);
            free(v);
        }
        free(c->name);
        free(c);

        c = unclog_global->configs.lh_first;
    }
}

// update the configuration store, this does not touch the sources or sinks,
// just the config
// - first clear up the config and install default config (this default might
//   be updated by the config but at least the default must be there)
// - if the input config pointer is not NULL, us that config
// - else search for files in unclog_config_locations
static void _unclog_config(const char* config) {
    _unclog_config_clear();

    unclog_config_t* c = _unclog_config_create("", UNCLOG_LEVEL_DEFAULT, UNCLOG_DETAILS_DEFAULT);
    LIST_INSERT_HEAD(&unclog_global->configs, c, entries);

    if (config != NULL) {
        ini_parse_string(config, _unclog_config_handler, NULL);
        return;
    }
    const char** f = unclog_config_locations;
    for (; *f != NULL; f++) {
        if (access(*f, R_OK) == 0) {
            fprintf(stderr, "read config from %s\n", *f);
            ini_parse(*f, _unclog_config_handler, NULL);
            return;
        }
    }
}

// gets or creates source. when the new source is created the config is fetched
// and the level is copied over
static unclog_source_t* _unclog_source_create_or_get(const char* source) {
    unclog_source_t* s = unclog_global->sources.lh_first;
    for (; s != NULL; s = s->entries.le_next) {
        if (MATCH(s->name, source)) break;
    }

    if (s == NULL) {
        unclog_config_t* config = _unclog_config_get("source", source, 0);

        s = malloc_and_clear(sizeof(unclog_source_t));
        s->name = strdup(source);
        s->level = config->level;

        LIST_INSERT_HEAD(&unclog_global->sources, s, entries);
    }

    return s;
}

// create, initialize and configure new logging single logging instance
static void unclog_global_create_and_lock(const char* config) {
    pthread_rwlock_wrlock(&unclog_mutex);
    if (unclog_global == NULL) {
        unclog_global = malloc_and_clear(sizeof(unclog_global_t));
        LIST_INIT(&unclog_global->configs);
        LIST_INIT(&unclog_global->sources);
        LIST_INIT(&unclog_global->sinks);

        _unclog_config(config);
    } else if (config != NULL) {
        _unclog_config(config);
    }
}

// initializes libary if not done, fetch or create source
unclog_t* unclog_open(const char* source) {
    unclog_global_create_and_lock(NULL);

    unclog_source_t* h = _unclog_source_create_or_get(source);

    pthread_rwlock_unlock(&unclog_mutex);
    return (unclog_t*)h;
}

// the actual logging call:
// - check if handle is valid, and if the level checks out
// - fill up data structure
// - call all the logging sinks
void unclog_log(unclog_data_t data, ...) {
    if (data.ha == NULL) return;
    unclog_source_t* source = (unclog_source_t*)data.ha;

    if (!UNCLOG_LEVEL_COMPARE(data.le, source->level)) return;

    time_t now = time(NULL);
    gmtime_r(&now, &data.ti);

    pthread_rwlock_rdlock(&unclog_mutex);
    unclog_sink_t* sink = unclog_global->sinks.lh_first;
    for (; sink != NULL; sink = sink->entries.le_next) {
        if (!UNCLOG_LEVEL_COMPARE(data.le, sink->level)) continue;
        data.da = sink->internal;
        va_list list;
        va_start(list, data);
        sink->methods.log(&data, list);
        va_end(list);
    }
    pthread_rwlock_unlock(&unclog_mutex);
}

// remove source and free up memory. if this was the last source, close down
// the library
void unclog_close(unclog_t* handle) {
    unclog_source_t* source = (unclog_source_t*)handle;

    pthread_rwlock_wrlock(&unclog_mutex);
    LIST_REMOVE(source, entries);
    free(source->name);
    free(source);

    if (unclog_global->sources.lh_first == NULL) {
        _unclog_config_clear();

        unclog_sink_t* s = unclog_global->sinks.lh_first;
        while (s != NULL) {
            unclog_sink_t* d = s;
            s = s->entries.le_next;

            d->methods.deinit(d->internal);
            free(d->name);
            free(d);
        }

        free(unclog_global);
        unclog_global = NULL;
    }
    pthread_rwlock_unlock(&unclog_mutex);
}

// register sink:
// - first make sure library is initialized
// - fetch config and create sink
// - depending on settings and parameters configure sink
// - call init method and store in list
void unclog_sink_register(const char* name, int level, uint32_t details,
                          unclog_sink_methods_t methods) {
    unclog_global_create_and_lock(NULL);

    unclog_config_t* c = _unclog_config_get("sink", name, 0);

    unclog_sink_t* sink = malloc_and_clear(sizeof(unclog_sink_t));
    if (level != UNCLOG_LEVEL_NONE)
        sink->level = level;
    else
        sink->level = c->level;
    if (details != UNCLOG_DETAILS_NONE)
        sink->details = details;
    else
        sink->details = c->details;
    sink->name = strdup(name);
    sink->methods = methods;

    sink->methods.init(&sink->internal, sink->details, c->config);

    LIST_INSERT_HEAD(&unclog_global->sinks, sink, entries);

    pthread_rwlock_unlock(&unclog_mutex);
}

void unclog_config(const char* config) {
    unclog_global_create_and_lock(config);

    unclog_source_t* s = unclog_global->sources.lh_first;
    for (; s != NULL; s = s->entries.le_next) {
        unclog_config_t* config = _unclog_config_get("source", s->name, 0);
        s->level = config->level;
    }
}
