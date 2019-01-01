#include <unclog_int.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    if (unclog_global != NULL) return -1;

    unclog_t* handle = unclog_open("herbert");
    if (handle == NULL) return -2;

    if (unclog_global == NULL) return -3;

    unclog_close(handle);

    if (unclog_global != NULL) return -4;
}
