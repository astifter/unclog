#include <unclog.h>

#include <stdio.h>

int main(int argc, char** argv) {
    fprintf(stderr,
            "--------------------------------------------------------------------------------\n");
    (void)argc;
    (void)argv;
    unclog_t* l1 = unclog_open("source1");
    UL_ERR(l1, "fritz: %d", 45);
    unclog_t* l2 = unclog_open("source2");
    UL_TRA(l2, "herbert");
    unclog_close(l1);
    unclog_close(l2);
    fprintf(stderr,
            "--------------------------------------------------------------------------------\n");
    return 0;
}
