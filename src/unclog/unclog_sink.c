#include <unclog_sink.h>

#include <unclog/unclog_adv.h>

#include <linux/limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOIL __attribute__((noinline))
//#define NOIL inline
#define MATCH(s1, s2) (strcmp(s1, s2) == 0)

static struct {
    int l;
    char c;
} unclog_level_tochars[] = {
    {0, '0'},
    {UNCLOG_LEVEL_FATAL, 'F'},
    {UNCLOG_LEVEL_CRITICAL, 'C'},
    {UNCLOG_LEVEL_ERROR, 'E'},
    {UNCLOG_LEVEL_WARNING, 'W'},
    {UNCLOG_LEVEL_INFO, 'I'},
    {UNCLOG_LEVEL_DEBUG, 'D'},
    {UNCLOG_LEVEL_TRACE, 'T'},
};

static NOIL char unclog_level_tochar(int l) {
    if (l < 0 || l > UNCLOG_LEVEL_TRACE) return unclog_level_tochars[0].c;
    return unclog_level_tochars[l].c;
}

static NOIL size_t stringappend(char* dest, const char* src) {
    size_t size = strlen(src);
    memcpy(dest, src, size + 1);
    return size;
}

static NOIL size_t unclog_sink_default(char* buffer, unclog_data_t* data, uint32_t details,
                                       va_list list) {
    char* bufferpos = buffer;
    uint32_t needsspace = 0;

    if (details & UNCLOG_DETAILS_TIMESTAMP) {
        bufferpos += strftime(bufferpos, 48, "%F %H:%M:%S", &data->ti);
        needsspace = 1;
    }
    if (details & UNCLOG_DETAILS_LEVEL) {
        if (needsspace) *(bufferpos++) = ' ';
        bufferpos += stringappend(bufferpos, "< >");
        *(bufferpos - 2) = unclog_level_tochar(data->le);
        needsspace = 1;
    }
    if (details & UNCLOG_DETAILS_SOURCE) {
        if (needsspace) *(bufferpos++) = ' ';
        bufferpos += stringappend(bufferpos, data->so);
        needsspace = 1;
    }
    if (details & UNCLOG_DETAILS_FILE) {
        if (needsspace) *(bufferpos++) = ' ';
        bufferpos += stringappend(bufferpos, data->fi);
        needsspace = 1;
    }
    if (details & UNCLOG_DETAILS_LINE) {
        if (needsspace) {
            if (details & UNCLOG_DETAILS_FILE)
                *(bufferpos++) = ':';
            else
                *(bufferpos++) = ' ';
        }
        bufferpos += sprintf(bufferpos, "%d", data->li);
        needsspace = 1;
    }
    if (details & UNCLOG_DETAILS_MESSAGE) {
        if (needsspace) *(bufferpos++) = ' ';
        const char* fmt = va_arg(list, char*);
        bufferpos += vsprintf(bufferpos, fmt, list);
        needsspace = 1;
    }
    *bufferpos++ = '\n';
    *bufferpos = '\0';

    return bufferpos - buffer;
}

static NOIL void unclog_sink_fprintf(const char* buffer) { fprintf(stderr, buffer); }

typedef struct unclog_sink_stderr_data_s { uint32_t details; } unclog_sink_stderr_data_t;

static uint64_t messages = 0;
static pthread_mutex_t message_mtx = PTHREAD_MUTEX_INITIALIZER;

static NOIL void unclog_sink_stderr_init(void** data, uint32_t details,
                                         unclog_config_value_t* config) {
    (void)config;

    unclog_sink_stderr_data_t* sink = malloc(sizeof(unclog_sink_stderr_data_t));
    sink->details = details;

    *data = sink;
}

static NOIL void unclog_sink_stderr_log(unclog_data_t* data, va_list list) {
    char buffer[PATH_MAX];
    *buffer = '\0';

    unclog_sink_stderr_data_t* sink = data->si;
    unclog_sink_default(buffer, data, sink->details, list);
    unclog_sink_fprintf(buffer);

    pthread_mutex_lock(&message_mtx);
    messages++;
    pthread_mutex_unlock(&message_mtx);
}

NOIL uint64_t unclog_sink_stderr_get_num_messages(void) { return messages; }

static NOIL void unclog_sink_stderr_deinit(void* data) { free(data); }

static char* unclog_sink_file_default_log = "unclog.log";

typedef struct unclog_sink_file_data_s {
    uint32_t details;
    FILE* file;
} unclog_sink_file_data_t;

static NOIL void unclog_sink_file_init(void** data, uint32_t details,
                                       unclog_config_value_t* config) {
    unclog_sink_file_data_t* sink = malloc(sizeof(unclog_sink_file_data_t));
    sink->details = details;
    sink->file = NULL;

    char* filename = unclog_config_value_get(config, "File");
    if (filename == NULL) filename = unclog_sink_file_default_log;
    if (filename != NULL) sink->file = fopen(filename, "ab");

    *data = sink;
}

static NOIL void unclog_sink_file_log(unclog_data_t* data, va_list list) {
    char buffer[PATH_MAX];
    *buffer = '\0';

    unclog_sink_file_data_t* sink = data->si;
    size_t size = unclog_sink_default(buffer, data, sink->details, list);
    fwrite(buffer, size, 1, sink->file);
}

static NOIL void unclog_sink_file_deinit(void* data) {
    unclog_sink_file_data_t* sink = data;
    if (sink->file != NULL) {
        fclose(sink->file);
        free(sink);
    }
}

static void NOIL unclog_sink_null_init(void** u1, uint32_t u2, unclog_config_value_t* u3) {
    (void)u1, (void)u2, (void)u3;
}

static void NOIL unclog_sink_null_log(unclog_data_t* u1, va_list u2) { (void)u1, (void)u2; }

static void NOIL unclog_sink_null_deinit(void* u1) { (void)u1; }

unclog_sink_list_t unclog_sink_list[] = {
    {"stderr", {unclog_sink_stderr_init, unclog_sink_stderr_log, unclog_sink_stderr_deinit}},
    {"file", {unclog_sink_file_init, unclog_sink_file_log, unclog_sink_file_deinit}},
    {"null", {unclog_sink_null_init, unclog_sink_null_log, unclog_sink_null_deinit}},
    {NULL, {NULL}},
};
