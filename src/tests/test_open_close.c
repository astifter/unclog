#include <unclog/unclog.h>
#include <unclog_test.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    if (unclog_test_is_initialized() != 0) return -1;

    unclog_t* handle = unclog_open("herbert");
    if (handle == NULL) return -2;

    if (unclog_test_is_initialized() == 0) return -3;

    unclog_close(handle);

    if (unclog_test_is_initialized() != 0) return -4;
}
