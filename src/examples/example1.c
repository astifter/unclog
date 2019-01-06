// only one include file is needed for basic usage
#include <unclog/unclog.h>

int main(int argc, char** argv) {
    // open a logging source "main", when this is called for the first time
    // during the process' lifetime the unclog.ini is searched for and
    // processed and unclog is initialized
    unclog_t* logger = unclog_open("main");
    // no create some logging messages on several logging levels for this
    // logging source
    UL_ER(logger, "error message: %d, logger is %p", 42, logger);
    UL_DE(logger, "debug message: %d, logger is %p", 43, logger);
    UL_TR(logger, "trace message: %d, logger is %p", 44, logger);
    // when the logger is not needed anymore it has to be closed. when this was
    // the last source to be closed unclog is de-initialized and all internal
    // memory is freed
    unclog_close(logger);
}
