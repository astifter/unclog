#define _DEFAULT_SOURCE

#include <unclog_int.h>

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

static void check_sink(const char* name, int level, uint32_t details, int allow_null_log) {
    unclog_sink_t* sink = unclog_global_sink_get(unclog_global, name);
    CU_ASSERT_PTR_NOT_NULL(sink);
    CU_ASSERT(sink->settings.level == level);
    CU_ASSERT(sink->settings.details == details);
    CU_ASSERT_PTR_NULL(sink->values);
    if (!allow_null_log) CU_ASSERT_PTR_NOT_NULL(sink->i->methods.log);
    CU_ASSERT_STRING_EQUAL(sink->i->sink, name);
}

static void check_source(const char* name, int level) {
    unclog_source_t* source = unclog_global_source_get(unclog_global, name);
    CU_ASSERT_PTR_NOT_NULL(source);
    CU_ASSERT(source->level == level);
    CU_ASSERT_STRING_EQUAL(source->source, name);
}

#define DEFINE_TEST(s, t) \
    { #s "_" #t, s##_##t }
#define DEFINE_SUITE(s) \
    { #s, s##_Init, NULL, s##_Tests }
#define DEFINE_SUITE_EXT(s, i, d) \
    { #s, i, d, s##_Tests }

static int initialization_Init(void) {
    unclog_deinit();
    if (unclog_global == NULL)
        return CUE_SUCCESS;
    else
        return CUE_SINIT_FAILED;
}

static void initialization_open_close(void) {
    unclog_t* handle1 = unclog_open("herbert");
    CU_ASSERT_PTR_NOT_NULL(unclog_global);
    CU_ASSERT_PTR_NOT_NULL(handle1);
    check_source("herbert", unclog_defaults.level);

    unclog_source_t* source = (unclog_source_t*)handle1;
    CU_ASSERT(source->level == unclog_defaults.level);
    CU_ASSERT(source->active == 1);
    CU_ASSERT(source->initialized == 0);
    CU_ASSERT_STRING_EQUAL(source->source, "herbert");

    unclog_t* handle2 = unclog_open("herbert");
    CU_ASSERT_PTR_NOT_NULL(handle2);
    CU_ASSERT_PTR_EQUAL(handle1, handle2);
    CU_ASSERT(source->active == 2);

    check_sink("stderr", unclog_defaults.level, unclog_defaults.details, 0);

    unclog_close(handle1);
    CU_ASSERT(source->active == 1);
    unclog_close(handle2);

    CU_ASSERT(unclog_global == NULL);
}

static void initialization_configuration_levels(void) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        char buffer[4096] = {0};
        sprintf(buffer, "[Defaults]\nLevel=%s\n", l->name);
        unclog_init(buffer);
        CU_ASSERT(unclog_global != NULL);
        CU_ASSERT(unclog_global->defaults.level == l->level);

        for (unclog_levels_t* n = unclog_levels; n->name != NULL; n++) {
            sprintf(buffer, "[Defaults]\nLevel=%s\n[stderr]\nLevel=%s\n", l->name, n->name);
            unclog_init(buffer);
            CU_ASSERT(unclog_global != NULL);
            CU_ASSERT(unclog_global->defaults.level == l->level);

            unclog_sink_t* s = unclog_global_sink_get(unclog_global, "stderr");
            CU_ASSERT(s != NULL);
            CU_ASSERT(s->settings.level == n->level);

            s = unclog_global_sink_get(unclog_global, "someothersink.so");
            CU_ASSERT(s == NULL);
        }

        for (unclog_levels_t* n = unclog_levels; n->name != NULL; n++) {
            sprintf(buffer,
                    "[Defaults]\nLevel=%s\nSinks=someothersink.so\n[someothersink.so]\nLevel=%s\n",
                    l->name, n->name);
            unclog_init(buffer);
            CU_ASSERT(unclog_global != NULL);
            CU_ASSERT(unclog_global->defaults.level == l->level);

            unclog_sink_t* s = unclog_global_sink_get(unclog_global, "someothersink.so");
            CU_ASSERT(s != NULL);
            CU_ASSERT(s->settings.level == n->level);

            s = unclog_global_sink_get(unclog_global, "stderr");
            CU_ASSERT(s == NULL);
        }
    }
}

static void initialization_configuration_details_single(uint32_t d, const char* n) {
    char* detailsstr = (char*)n;
    if (n == NULL) detailsstr = unclog_details_tostr(d);

    char buffer[4096] = {0};
    sprintf(buffer, "[Defaults]\nDetails=%s\n", detailsstr);
    unclog_init(buffer);
    CU_ASSERT(unclog_global != NULL);
    CU_ASSERT(unclog_global->defaults.details == d);

    if (n == NULL) free(detailsstr);
}

static void initialization_configuration_details_all(void) {
    for (unclog_details_t* l = unclog_details; l->name != NULL; l++) {
        initialization_configuration_details_single(l->detail, l->name);
    }
}

static void initialization_configuration_details_random(void) {
    unsigned int MAX = 0;
    unsigned int MIN = UINT_MAX;
    for (int i = 0; i < 100; i++) {
        unsigned int r = random() * (double)(UNCLOG_OPT_MAXIMUM + 1) / RAND_MAX;
        if (r > MAX) MAX = r;
        if (r < MIN) MIN = r;

        initialization_configuration_details_single(r, NULL);
    }
    initialization_configuration_details_single(MIN, NULL);
    initialization_configuration_details_single(MAX, NULL);
}

static void initialization_configuration_reinit(void) {
    fprintf(stderr, "\n");

    unclog_init(
        "[Defaults]\n"
        "Level=Debug\n"
        "Details=Time\n"
        "Sinks=stderr,newsink\n"
        "[stderr]\n"
        "Level=Trace\n"
        "Details=Message\n"
        "[logger1]\n"
        "Level=Error\n");
    unclog_t* logger1 = unclog_open("logger1");
    unclog_t* logger2 = unclog_open("logger2");
    unclog_sink_register("newsink", NULL, (unclog_sink_methods_t){0});
    unclog_global_dump_config(unclog_global);

    CU_ASSERT(unclog_global->defaults.level == UNCLOG_LEVEL_DEBUG);
    CU_ASSERT(unclog_global->defaults.details == UNCLOG_OPT_TIMESTAMP);
    check_sink("stderr", UNCLOG_LEVEL_TRACE, UNCLOG_OPT_MESSAGE, 0);
    check_sink("newsink", UNCLOG_LEVEL_DEBUG, UNCLOG_OPT_TIMESTAMP, 1);
    check_source("logger1", UNCLOG_LEVEL_ERROR);
    check_source("logger2", UNCLOG_LEVEL_DEBUG);

    unclog_reinit("");
    unclog_global_dump_config(unclog_global);

    CU_ASSERT(unclog_global->defaults.level == unclog_defaults.level);
    CU_ASSERT(unclog_global->defaults.details == unclog_defaults.details);
    check_sink("stderr", unclog_defaults.level, unclog_defaults.details, 0);
    check_sink("newsink", unclog_defaults.level, unclog_defaults.details, 1);
    check_source("logger1", unclog_defaults.level);
    check_source("logger2", unclog_defaults.level);

    unclog_close(logger1);
    unclog_close(logger2);
    unclog_deinit();
}

static uint32_t initialization_configuration_register_called = 0;
void initialization_configuration_register_init(unclog_sink_t* s) {
    (void)s;
    initialization_configuration_register_called |= 0x01;
}
void initialization_configuration_register_log(unclog_data_t* d, va_list l) {
    (void)d;
    (void)l;
    initialization_configuration_register_called |= 0x04;
}
void initialization_configuration_register_deinit(unclog_sink_t* s) {
    (void)s;
    initialization_configuration_register_called |= 0x02;
}

void initialization_configuration_register(void) {
    unclog_t* log = unclog_open("herbert");
    unclog_values_t settings = {.level = UNCLOG_LEVEL_TRACE};
    unclog_sink_register("newsink", &settings,
                         (unclog_sink_methods_t){initialization_configuration_register_init,
                                                 initialization_configuration_register_log,
                                                 initialization_configuration_register_deinit});
    CU_ASSERT(initialization_configuration_register_called == 0x01);
    UL_CR(log, "fritz");
    CU_ASSERT(initialization_configuration_register_called == 0x05);
    unclog_deinit();
    CU_ASSERT(initialization_configuration_register_called == 0x07);
}

static CU_TestInfo initialization_Tests[] = {
    DEFINE_TEST(initialization, open_close),
    DEFINE_TEST(initialization, configuration_levels),
    DEFINE_TEST(initialization, configuration_details_all),
    DEFINE_TEST(initialization, configuration_details_random),
    DEFINE_TEST(initialization, configuration_reinit),
    DEFINE_TEST(initialization, configuration_register),
    CU_TEST_INFO_NULL,
};

static unclog_values_t logging_sink_settings = {
    .level = UNCLOG_LEVEL_TRACE, .details = UNCLOG_OPT_MESSAGE,
};

static int logging_sink_counter_is = 0;
static int logging_sink_counter_should = 0;
static void logging_sink(unclog_data_t* d, va_list list) {
    unclog_source_t* source = (unclog_source_t*)d->ha;
    fprintf(stderr, "ha: %p, le: %s, fi: %s, fu: %s, li: %d, src: %s, msg: ", (void*)d->ha,
            unclog_level_tostr(d->le), d->fi, d->fu, d->li, source->source);
    const char* fmt = va_arg(list, char*);
    if (fmt != NULL) vfprintf(stderr, fmt, list);
    fprintf(stderr, "\n");
    logging_sink_counter_is++;
}

static int logging_simple_Init(void) {
    unclog_deinit();
    if (unclog_global != NULL) return CUE_SINIT_FAILED;
    unclog_init(
        "[Defaults]\n"
        "Level=Error\n"
        "Sinks=Testing\n"
        "[Testing]\n"
        "Level=Warning\n"
        "[testing]\n"
        "Level=Trace\n");
    // fprintf(stderr, "\n");
    // unclog_global_dump_config(unclog_global);
    if (unclog_global == NULL) return CUE_SINIT_FAILED;

    unclog_sink_register("Testing", &logging_sink_settings,
                         (unclog_sink_methods_t){.log = logging_sink});

    return CUE_SUCCESS;
}

static int logging_simple_Deinit(void) {
    unclog_deinit();
    if (unclog_global != NULL)
        return CUE_SINIT_FAILED;
    else
        return CUE_SUCCESS;
}

static void logging_simple_check_sink(void) {
    check_sink("Testing", logging_sink_settings.level, logging_sink_settings.details, 0);
    fprintf(stderr, "\n");
    unclog_global_dump_config(unclog_global);
}

static void logging_simple_primitive(void) {
    fprintf(stderr, "\n");
    unclog_t* logger = unclog_open("testing");
    UL_FA(logger, "Fatal message");
    UL_CR(logger, "Critical message");
    UL_ER(logger, "Error message");
    UL_WA(logger, "Warning message");
    UL_IN(logger, "Info message");
    UL_DE(logger, "Debug message");
    UL_TR(logger, "Trace message");
    unclog_close(logger);
}

static void logging_simple_loop(void) {
    fprintf(stderr, "\n");
    unclog_t* logger = unclog_open("testing");
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        char buffer[4096] = {0};
        sprintf(buffer, "%s message", l->name);
        UNCLOG(logger, l->level, buffer);
    }
    unclog_close(logger);
}

static CU_TestInfo logging_simple_Tests[] = {
    DEFINE_TEST(logging_simple, check_sink), DEFINE_TEST(logging_simple, primitive),
    DEFINE_TEST(logging_simple, loop), CU_TEST_INFO_NULL,
};

static int logging_complex_Init(void) { return CUE_SUCCESS; }

static unclog_t* logging_complex_logger = NULL;
static void logging_complex_setup(int level_source, int level_sink) {
    logging_sink_counter_is = 0;
    logging_sink_counter_should = 0;

    unclog_init(
        "[Defaults]\n"
        "Sinks=Testing\n");
    unclog_values_t settings = {
        .level = level_sink,
    };
    unclog_sink_register("Testing", &settings, (unclog_sink_methods_t){.log = logging_sink});
    check_sink("Testing", level_sink, unclog_defaults.details, 0);

    logging_complex_logger = unclog_open("testing");
    logging_complex_logger->level = level_source;
}

static void logging_complex_loop1(void) {
    fprintf(stderr, "\n");
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        logging_complex_setup(l->level, UNCLOG_LEVEL_TRACE);

        for (unclog_levels_t* s = unclog_levels; s->name != NULL; s++) {
            UNCLOG(logging_complex_logger, s->level, "herbert");
            if (s->level <= l->level) logging_sink_counter_should++;
            CU_ASSERT(logging_sink_counter_is == logging_sink_counter_should);
        }
        fprintf(stderr, "is: %d, should: %d\n", logging_sink_counter_is,
                logging_sink_counter_should);

        unclog_close(logging_complex_logger);

        unclog_deinit();
    }
}

static void logging_complex_loop2(void) {
    fprintf(stderr, "\n");
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        logging_complex_setup(l->level, UNCLOG_LEVEL_TRACE);

        for (unclog_levels_t* s = unclog_levels; s->name != NULL; s++) {
            UNCLOG(logging_complex_logger, s->level, "herbert");
            if (s->level <= l->level) logging_sink_counter_should++;
            CU_ASSERT(logging_sink_counter_is == logging_sink_counter_should);
        }
        fprintf(stderr, "is: %d, should: %d\n", logging_sink_counter_is,
                logging_sink_counter_should);

        unclog_close(logging_complex_logger);

        unclog_deinit();
    }
}

static void logging_complex_file_sink_1(void) {
    unclog_init("[Defaults]\nSinks=file\n[file]\nFile=unclog.log");
    unclog_t* log = unclog_open("fritz");
    UL_CR(log, "herbert");
    unclog_deinit();
}

static void logging_complex_file_sink_2(void) {
    unclog_init("[Defaults]\nSinks=file");
    unclog_t* log = unclog_open("fritz");
    UL_CR(log, "herbert");
    unclog_deinit();
}

static CU_TestInfo logging_complex_Tests[] = {
    DEFINE_TEST(logging_complex, loop1),
    DEFINE_TEST(logging_complex, loop2),
    DEFINE_TEST(logging_complex, file_sink_1),
    DEFINE_TEST(logging_complex, file_sink_2),
    CU_TEST_INFO_NULL,
};

static int logging_manual_Init(void) { return CUE_SUCCESS; }

static void logging_manual_details(void) {
    fprintf(stderr, "\n");
    for (int i = 0; i <= UNCLOG_OPT_MAXIMUM; i++) {
        char* detailsstr = unclog_details_tostr(i);
        fprintf(stderr, "DETAILS: %d | %s\n", i, detailsstr);
        free(detailsstr);

        unclog_t* logger = unclog_open("main");
        unclog_sink_t* sink = unclog_global_sink_get(unclog_global, "stderr");
        sink->settings.details = i;
        sink->settings.level = UNCLOG_LEVEL_TRACE;

        for (unclog_levels_t* s = unclog_levels; s->name != NULL; s++) {
            UNCLOG(logger, s->level, "herbert");
        }
        unclog_close(logger);
    }
}

static CU_TestInfo logging_manual_Tests[] = {
    DEFINE_TEST(logging_manual, details), CU_TEST_INFO_NULL,
};

static CU_SuiteInfo suites[] = {
    DEFINE_SUITE(initialization),
    DEFINE_SUITE_EXT(logging_simple, logging_simple_Init, logging_simple_Deinit),
    DEFINE_SUITE(logging_complex),
    DEFINE_SUITE(logging_manual),
    CU_SUITE_INFO_NULL,
};

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    srandom(time(NULL));

    CU_initialize_registry();

    CU_register_suites(suites);
    // CU_automated_run_tests();
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures;
}
