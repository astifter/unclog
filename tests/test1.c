#include <unclog.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* test_thread(void* data) {
	char buffer[12] = {0};
	sprintf(buffer, "thread %d", *(int*)data);
	unclog_t* l = unclog_open(buffer);
	for(int i = 0; i < 7; i++) {
		int level = (i*200)+200;
		unclog_log(l, level, __FILE__, __func__, __LINE__, "test level %d", level);
		sleep(1);
	}
	unclog_close(l);
}

int main(int argc, char** argv) {
    fprintf(stderr,
            "--------------------------------------------------------------------------------\n");
    (void)argc;
    (void)argv;
    unclog_t* l1 = unclog_open("source1");
    fprintf(stderr,
            "--------------------------------------------------------------------------------\n");
    UL_ERR(l1, "fritz: %d", 45);
    unclog_t* l2 = unclog_open("source2");
    UL_TRA(l2, "herbert");
    unclog_close(l1);
    unclog_close(l2);
    fprintf(stderr,
            "--------------------------------------------------------------------------------\n");
#define THREAD_COUNT 10
	pthread_t threads[THREAD_COUNT];
	int thread_data[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; i++) {
		thread_data[i] = i;
		pthread_create(&threads[i], NULL, test_thread, &thread_data[i]);
	}
	for (int i = 0; i < THREAD_COUNT; i++) {
		pthread_join(threads[i], NULL);
	}
    fprintf(stderr,
            "--------------------------------------------------------------------------------\n");
    return 0;
}
