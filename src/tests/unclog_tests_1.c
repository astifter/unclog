#include <unclog_int.h>

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#define DEFINE_TEST(s, t) \
    { #s "_" #t, s##_##t }
#define DEFINE_SUITE(s) \
    { #s, NULL, NULL, s##_Tests }

void initialization_open_close(void) {
    fprintf(stderr, "%s\n", __func__);
    CU_ASSERT(unclog_global == NULL);

    unclog_t* handle = unclog_open("herbert");
    CU_ASSERT(unclog_global != NULL);
    CU_ASSERT(handle != NULL);
    unclog_global_dump_config(unclog_global);

    unclog_close(handle);
    CU_ASSERT(unclog_global == NULL);
}

void initialization_configuration(void) {
    fprintf(stderr, "%s\n", __func__);
    CU_ASSERT(1);
}

CU_TestInfo initialization_Tests[] = {
    DEFINE_TEST(initialization, open_close), DEFINE_TEST(initialization, configuration),
    CU_TEST_INFO_NULL,
};

CU_SuiteInfo suites[] = {
    DEFINE_SUITE(initialization), CU_SUITE_INFO_NULL,
};

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    CU_initialize_registry();

    CU_register_suites(suites);
    // CU_automated_run_tests();
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures;
}
