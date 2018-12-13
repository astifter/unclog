#include <unclog.h>

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
    unclog_t* l = unclog_open("source1");
    UL_ERR(l, "fritz: %d", 45);
	UL_TRA(l, "herbert");
	unclog_close(l);
    return 0;
}
