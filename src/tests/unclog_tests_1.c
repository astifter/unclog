#include <unclog_int.h>

#include <CUnit/CUnit.h>

#define DEFINE_TEST(s, t) \
    { #s "_" #t, s##_##t }
#define DEFINE_SUITE(s) \
    { #s, s##_Init, s##_Cleanup, s##_Tests }

void initialization_test1(void) {
    fprintf(stderr, "%s\n", __func__);
    CU_ASSERT(1);
}

int initialization_Init(void) {
    fprintf(stderr, "%s\n", __func__);
    return CUE_SUCCESS;
}

int initialization_Cleanup(void) {
    fprintf(stderr, "%s\n", __func__);
    return CUE_SUCCESS;
}

CU_TestInfo initialization_Tests[] = {
    DEFINE_TEST(initialization, test1), CU_TEST_INFO_NULL,
};

CU_SuiteInfo suites[] = {
    DEFINE_SUITE(initialization), CU_SUITE_INFO_NULL,
};

int main(int argc, char** argv) {
    CU_initialize_registry();

    CU_register_suites(suites);
    CU_automated_run_tests();

    int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures;
}
