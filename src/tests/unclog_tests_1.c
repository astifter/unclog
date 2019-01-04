#define _BSD_SOURCE

#include <unclog_int.h>

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <stdlib.h>

#define DEFINE_TEST(s, t) \
    { #s "_" #t, s##_##t }
#define DEFINE_SUITE(s) \
    { #s, s##_Init, NULL, s##_Tests }

int initialization_Init(void) {
    unclog_deinit();
    if (unclog_global == NULL)
        return CUE_SUCCESS;
    else
        return CUE_SINIT_FAILED;
}

void initialization_open_close(void) {
    unclog_t* handle1 = unclog_open("herbert");
    CU_ASSERT_PTR_NOT_NULL(unclog_global);
    CU_ASSERT_PTR_NOT_NULL(handle1);

    unclog_source_t* source = (unclog_source_t*)handle1;
    CU_ASSERT(source->level == unclog_defaults.level);
    CU_ASSERT(source->active == 1);
    CU_ASSERT_STRING_EQUAL(source->source, "herbert");

    unclog_t* handle2 = unclog_open("herbert");
    CU_ASSERT_PTR_NOT_NULL(handle2);
    CU_ASSERT_PTR_EQUAL(handle1, handle2);
    CU_ASSERT(source->active == 2);

    unclog_sink_t* sink = unclog_global_sink_get(unclog_global, "libunclog_stderr.so");
    CU_ASSERT_PTR_NOT_NULL(sink);
    CU_ASSERT(sink->settings.level == unclog_defaults.level);
    CU_ASSERT(sink->settings.details == unclog_defaults.details);
    CU_ASSERT_PTR_NULL(sink->values);
    CU_ASSERT_PTR_NOT_NULL(sink->log);
    CU_ASSERT_STRING_EQUAL(sink->sink, "libunclog_stderr.so");

    unclog_close(handle1);
    CU_ASSERT(source->active == 1);
    unclog_close(handle2);

    CU_ASSERT(unclog_global == NULL);
}

void initialization_configuration_levels(void) {
    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        char buffer[4096] = {0};
        sprintf(buffer, "[Defaults]\nLevel=%s\n", l->name);
        unclog_init(buffer);
        CU_ASSERT(unclog_global != NULL);
        CU_ASSERT(unclog_global->defaults.level == l->level);

        for (unclog_levels_t* n = unclog_levels; n->name != NULL; n++) {
            sprintf(buffer, "[Defaults]\nLevel=%s\n[libunclog_stderr.so]\nLevel=%s\n", l->name,
                    n->name);
            unclog_init(buffer);
            CU_ASSERT(unclog_global != NULL);
            CU_ASSERT(unclog_global->defaults.level == l->level);

            unclog_sink_t* s = unclog_global_sink_get(unclog_global, "libunclog_stderr.so");
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

            s = unclog_global_sink_get(unclog_global, "libunclog_stderr.so");
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

void initialization_configuration_details_all(void) {
    for (unclog_details_t* l = unclog_details; l->name != NULL; l++) {
        initialization_configuration_details_single(l->detail, l->name);
    }
}

void initialization_configuration_details_random(void) {
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

void initialization_configuration_details_sync(void) { CU_ASSERT(1); }

CU_TestInfo initialization_Tests[] = {
    DEFINE_TEST(initialization, open_close),
    DEFINE_TEST(initialization, configuration_levels),
    DEFINE_TEST(initialization, configuration_details_all),
    DEFINE_TEST(initialization, configuration_details_random),
    DEFINE_TEST(initialization, configuration_details_sync),
    CU_TEST_INFO_NULL,
};

CU_SuiteInfo suites[] = {
    DEFINE_SUITE(initialization), CU_SUITE_INFO_NULL,
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
