#include "unclog_test.h"
#include "unclog_int.h"

int unclog_test_is_initialized(void) { return unclog_global != NULL; }

int unclog_test_is_level(int level) { return unclog_global->defaults.level == level; }
