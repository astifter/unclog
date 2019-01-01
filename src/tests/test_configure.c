#include <unclog/unclog_adv.h>
#include <unclog_test.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    char buffer[4096] = {0};

    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (unclog_test_is_initialized() != 0) return -1;

        sprintf(buffer, "[Defaults]\nLevel=%s\n", l->name);
        unclog_init(buffer);
        if (unclog_test_is_initialized() == 0) return -2;
        if (unclog_test_is_level(l->level) != 1) return -3;

        unclog_levels_t* n = l + 1;
        if (n->name != NULL) {
            sprintf(buffer, "[Defaults]\nLevel=%s\n[libunclog_stderr.so]\nLevel=Trace", n->name);
            unclog_init(buffer);
            if (unclog_test_is_initialized() == 0) return -4;
            if (unclog_test_is_level(n->level) != 1) return -5;
        }

        unclog_deinit();
        if (unclog_test_is_initialized() != 0) return -5;
    }

    return 0;
}
