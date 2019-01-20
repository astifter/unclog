// only one include file is needed for basic usage
#include <unclog/unclog_adv.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    // open a logging source "main", when this is called for the first time
    // during the process' lifetime the unclog.ini is searched for and
    // processed and unclog is initialized
    unclog_t* logger = unclog_open("main");
    // no create some logging messages on several logging levels for this
    // logging source
    UL_FA(logger, "FATAL %d, logger is %p", 42, logger);
    UL_CR(logger, "CRITICAL %d, logger is %p", 43, logger);
    UL_ER(logger, "ERROR %d, logger is %p", 44, logger);
    UL_WA(logger, "WARNING %d, logger is %p", 45, logger);
    UL_IN(logger, "INFO %d, logger is %p", 46, logger);
    UL_DE(logger, "DEBUG %d, logger is %p", 47, logger);
    UL_TR(logger, "TRACE %d, logger is %p", 48, logger);
    // when the logger is not needed anymore it has to be closed. when this was
    // the last source to be closed unclog is de-initialized and all internal
    // memory is freed
    unclog_close(logger);
}
