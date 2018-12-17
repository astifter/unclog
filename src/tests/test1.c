#define _BSD_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <unclog/unclog.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct test_thread_param_s {
    int thread;
    int use_same_id;
    int use_random_wait;
    int use_random_level;
} test_thread_param_t;

static void* test_thread(void* data) {
    test_thread_param_t* param = data;

    char buffer[12] = {0};
    if (param->use_same_id == 1)
        sprintf(buffer, "thread same");
    else
        sprintf(buffer, "thread %d", param->thread);
    unclog_t* l = unclog_open(buffer);

    for (int i = 0; i < 7; i++) {
        int level = (i * 200) + 200;
        if (param->use_random_level) level = random() * 1600.0 / RAND_MAX;

        struct timespec wait = {1, 0};
        if (param->use_random_wait == 1) {
            wait.tv_sec = 0;
            wait.tv_nsec = random() * 1000000000.0 / RAND_MAX;
        }
        unclog_log(l, level, __FILE__, __func__, __LINE__, "test level %4d, sleep: %ld.%09ld",
                   level, wait.tv_sec, wait.tv_nsec);
        nanosleep(&wait, NULL);
    }
    unclog_close(l);

    return NULL;
}

#define THREAD_COUNT 10
static test_thread_param_t test_params[] = {
    {0, 1, 1, 1}, {0, 1, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 1},
    {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}, {1, 0, 0, 0},
};

int main(int argc, char** argv) {
    srandom(time(NULL));
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
    for (test_thread_param_t* p = test_params; p->thread != 1; p++) {
        pthread_t threads[THREAD_COUNT];
        test_thread_param_t thread_data[THREAD_COUNT];
        for (int i = 0; i < THREAD_COUNT; i++) {
            memcpy(&thread_data[i], p, sizeof(test_thread_param_t));
            thread_data[i].thread = i;
            pthread_create(&threads[i], NULL, test_thread, &thread_data[i]);
        }
        for (int i = 0; i < THREAD_COUNT; i++) {
            pthread_join(threads[i], NULL);
        }
        fprintf(
            stderr,
            "--------------------------------------------------------------------------------\n");
    }
    return 0;
}
