#include <unclog_int.h>

#include <stdio.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    char buffer[4096] = {0};

    for (unclog_levels_t* l = unclog_levels; l->name != NULL; l++) {
        if (unclog_global != NULL) return -1;

        sprintf(buffer, "[Defaults]\nLevel=%s\n", l->name);
        unclog_init(buffer);
        if (unclog_global == NULL) return -2;
        if (unclog_global->defaults.level != l->level) return -3;

    	for (unclog_levels_t* n = unclog_levels; n->name != NULL; n++) {
            sprintf(buffer, "[Defaults]\nLevel=%s\nSinks=someothersink.so\n[someothersink.so]\nLevel=%s\n", l->name, n->name);
            unclog_init(buffer);
        	if (unclog_global == NULL) return -4;
        	if (unclog_global->defaults.level != l->level) return -5;

			unclog_sink_t* s = unclog_global_sink_get(unclog_global, "someothersink.so");
			if (s == NULL) return -6;
			if (s->common.level != n->level) return -7;
        }

    	//for (unclog_levels_t* n = unclog_levels; n->name != NULL; n++) {
        //    sprintf(buffer, "[Defaults]\nLevel=%s\nSinks=libunclog_stderr.so,someothersink.so\n[libunclog_stderr.so]\nLevel=%s\n[someothersink.so]\nLevel=%s\n", l->name, n->name, l->name);
        //    unclog_init(buffer);
        //    if (unclog_test_is_level(l->level) != 1) return -5;
        //}

        unclog_deinit();
       	if (unclog_global != NULL) return -5;
    }

    return 0;
}
