#define _BSD_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <unclog/unclog.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct test_thread_param_s {
    int thread;
    int use_same_id;
    int use_random_wait;
    int use_random_level;
} test_thread_param_t;

#define THREAD_COUNT 5
static test_thread_param_t test_params[] = {
    {0, 1, 1, 1}, {0, 1, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 1},
    {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}, {1, 0, 0, 0},
};

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
        unclog_log(
            (unclog_data_t){.ha = l, .le = level, .fi = __FILE__, .fu = __func__, .li = __LINE__},
            "test level %4d, sleep: %ld.%09ld", level, wait.tv_sec, wait.tv_nsec);
        nanosleep(&wait, NULL);
    }
    unclog_close(l);

    return NULL;
}

void* sleepthread(void* data) {
    int* shutdown = (void*)data;
    struct timespec sleep = {15, 0};
    nanosleep(&sleep, NULL);
    *shutdown = 1;
    return NULL;
}

struct timespec timespec_diff(struct timespec t1, struct timespec t2) {
    long nsecs = t1.tv_nsec - t2.tv_nsec;
    if (nsecs < 0) {
        nsecs += 1000000000;
        t1.tv_sec -= 1;
    }
    long nsecs_rem = nsecs / 1000000000;
    t1.tv_sec -= nsecs_rem;
    long secs = t1.tv_sec - t2.tv_sec;
    return (struct timespec){secs, nsecs};
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    struct timespec start, stop, diff;
    fprintf(stderr, "----------------------------------------------------------------------\n");

    unclog_t* l1 = unclog_open("source1");
    UL_ER(l1, "fritz: %d", 45);
    unclog_t* l2 = unclog_open("source2");
    UL_TR(l2, "herbert");
    unclog_close(l1);
    unclog_close(l2);
    fprintf(stderr, "----------------------------------------------------------------------\n");

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
        fprintf(stderr, "----------------------------------------------------------------------\n");
    }

    int shutdown = 0;
    pthread_t timerthread;
    pthread_create(&timerthread, NULL, sleepthread, &shutdown);

    unclog_t* l = unclog_open("speed");
    clock_gettime(CLOCK_MONOTONIC, &start);
    int i = 0;
    while (shutdown == 0) {
        UL_ER(l, "logging %d", i);
        i++;
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    diff = timespec_diff(stop, start);
    fprintf(stderr, "start %ld.%09ld seconds\n", start.tv_sec, start.tv_nsec);
    fprintf(stderr, "stop %ld.%09ld seconds\n", stop.tv_sec, stop.tv_nsec);

    double msg_sec = i / (diff.tv_sec + (diff.tv_nsec / 1000000000.0));
    char* postfix = "";
    if (msg_sec > 1000) {
        msg_sec /= 1000;
        postfix = "k";
    }
    if (msg_sec > 1000) {
        msg_sec /= 1000;
        postfix = "M";
    }

    fprintf(stderr, "%d messages took %ld.%09ld seconds: %.3lf%s msgs/sec\n", i, diff.tv_sec,
            diff.tv_nsec, msg_sec, postfix);
    fprintf(stderr, "----------------------------------------------------------------------\n");
    return 0;
}
