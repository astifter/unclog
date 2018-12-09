#include <unclog.h>

int main(unsigned int argc, char** argv) {
    void* l = unclog_open("source1");
    UL_ERR(l, "fritz");
    return 0;
}
