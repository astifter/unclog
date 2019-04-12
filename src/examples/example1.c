#define _GNU_SOURCE

// include advanced header for using custom sink, this also includes the
// regular unclog/unclog.h
#include <unclog/unclog_adv.h>

#include <stdio.h>
#include <stdlib.h>

// create custom data structure for holding data for custom sink
typedef struct customsink_data_s { uint32_t details; } customsink_data_t;

// the initalization for the custom sink, data can be stored to the data
// pointer
void customsink_init(void** data, uint32_t details, unclog_config_value_t* config) {
    customsink_data_t* sink = malloc(sizeof(customsink_data_t));
    sink->details = details;
    *data = sink;

    fprintf(stderr, "%s:%d initialized with details 0x%08u\n", __FUNCTION__, __LINE__,
            sink->details);
    for (unclog_config_value_t* v = config; v != NULL; v = v->next) {
        fprintf(stderr, "%s:%d initialized with config %s=%s\n", __FUNCTION__, __LINE__, v->name,
                v->value);
    }
}

// the logging method, data contains all information necessary to process the
// logging event, list contains the va_list from unclog_log
// the data stored in the init method is found in data->data
void customsink_log(unclog_data_t* data, va_list list) {
    customsink_data_t* sink = data->si;

    char* msg;
    char* fmt = va_arg(list, char*);
    vasprintf(&msg, fmt, list);
    fprintf(stderr, "%s:%d log 0x%08u %s\n", __FUNCTION__, __LINE__, sink->details, msg);
    free(msg);
}

// clean up sink and open resources
void customsink_deinit(void* data) {
    fprintf(stderr, "%s:%d deinitialized\n", __FUNCTION__, __LINE__);
    free(data);
}

// collect methods for registering sink
unclog_sink_methods_t customsink = {customsink_init, customsink_log, customsink_deinit};

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    // first register a custom sink, since unclog is not initialized internally
    // the initialization is done as well
    unclog_sink_register_or_get("custom1", UNCLOG_LEVEL_NONE, UNCLOG_DETAILS_NONE, customsink);

    // open a logging source "main", when this is called for the first time
    // during the process' lifetime the unclog.ini is searched for and
    // processed and unclog is initialized
    unclog_t* logger1 = unclog_open("log1");

    // now create some logging messages on several logging levels for this
    // logging source
    UL_FA(logger1, "FATAL %d, logger1 is %p", 42, logger1);
    UL_CR(logger1, "CRITICAL %d, logger1 is %p", 43, logger1);
    UL_ER(logger1, "ERROR %d, logger1 is %p", 44, logger1);
    UL_WA(logger1, "WARNING %d, logger1 is %p", 45, logger1);
    UL_IN(logger1, "INFO %d, logger1 is %p", 46, logger1);
    UL_DE(logger1, "DEBUG %d, logger1 is %p", 47, logger1);
    UL_TR(logger1, "TRACE %d, logger1 is %p", 48, logger1);

    // create a new logger for later
    unclog_t* logger2 = unclog_open("log2");

    // when the logger1 is not needed anymore it has to be closed. when this was
    // the last source to be closed unclog is de-initialized and all internal
    // memory is freed
    unclog_close(logger1);

    // log to second logger
    UL_FA(logger2, "FATAL %d, logger2 is %p", 42, logger2);
    UL_CR(logger2, "CRITICAL %d, logger2 is %p", 43, logger2);
    UL_ER(logger2, "ERROR %d, logger2 is %p", 44, logger2);
    UL_WA(logger2, "WARNING %d, logger2 is %p", 45, logger2);
    UL_IN(logger2, "INFO %d, logger2 is %p", 46, logger2);
    UL_DE(logger2, "DEBUG %d, logger2 is %p", 47, logger2);
    UL_TR(logger2, "TRACE %d, logger2 is %p", 48, logger2);

    // reconfigure
    unclog_config("[defaults]\nLevel=Error");

    // again log to second logger
    UL_FA(logger2, "FATAL %d, logger2 is %p", 42, logger2);
    UL_CR(logger2, "CRITICAL %d, logger2 is %p", 43, logger2);
    UL_ER(logger2, "ERROR %d, logger2 is %p", 44, logger2);
    UL_WA(logger2, "WARNING %d, logger2 is %p", 45, logger2);
    UL_IN(logger2, "INFO %d, logger2 is %p", 46, logger2);
    UL_DE(logger2, "DEBUG %d, logger2 is %p", 47, logger2);
    UL_TR(logger2, "TRACE %d, logger2 is %p", 48, logger2);

    // now close logger2 as well
    unclog_close(logger2);

    // when unclog_sink_register() or unclog_init()/unclog_config() are used
    // unclog has to be deinitialized as well
    unclog_deinit();
    // all internal logging resources are freed now
}
