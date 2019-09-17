#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <unclog_int.h>

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WAITFLAG_RANDOM 0x0001
#define WAITFLAG_WAIT 0x0002
typedef struct test_thread_param_s {
    pthread_t thread;
    int threadid;
    uint32_t waiting_flags;
    uint64_t messages;
} test_thread_param_t;

test_thread_param_t default_params = {0, 0, 0, 0};
static int THREAD_COUNT = 10;

static void* test_thread(void* data) {
    test_thread_param_t* p = data;

    unclog_t* l = unclog_open("thread");

    for (int i = 0; p->threadid > 0; i++) {
        int level = UNCLOG_LEVEL_TRACE;  // = 1 + i % 7;

        struct timespec wait = {1, 0};
        if (p->waiting_flags & WAITFLAG_RANDOM) {
            wait.tv_sec = 0;
            wait.tv_nsec = random() * 100000000.0 / RAND_MAX;
        }
        unclog_log(
            (unclog_data_t){.ha = l, .le = level, .fi = __FILE__, .fu = __func__, .li = __LINE__},
            "thread %3d, test level %4d, sleep: %ld.%09ld", p->threadid, level, wait.tv_sec,
            wait.tv_nsec);
        p->messages++;

        if (p->waiting_flags & WAITFLAG_WAIT) nanosleep(&wait, NULL);
    }

    unclog_close(l);

    return NULL;
}

void struct_timespec_add_ms(struct timespec* n, unsigned long time_ms) {
    if (time_ms == 0) return;
    n->tv_sec += (time_ms / 1000);
    n->tv_nsec += ((time_ms * 1000000) % 1000000000);
    if (n->tv_nsec >= 1000000000) {
        n->tv_nsec -= 1000000000;
        n->tv_sec += 1;
    }
}

void* sleepthread(void* data) {
    unclog_t* logger = unclog_open("sleep");

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    char buffer[4096] = "";
    for (int i = 0; i < 30; i++) {
        // int level = 1 + i % 7;
        *buffer = '\0';
        sprintf(buffer,
                "[Defaults]\nDetails=Full\nLevel=Trace\nSinks=stderr\n[thread]\nLevel=Trace");

        // unclog_global_dump_config(unclog_global);
        UL_FA(logger, "re-configuring logger:\n%s", buffer);
        unclog_reinit(buffer);
        // unclog_global_dump_config(unclog_global);

        struct_timespec_add_ms(&now, 1000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &now, NULL);
    }

    test_thread_param_t* params = data;
    for (int i = 0; i < THREAD_COUNT; i++) {
        test_thread_param_t* p = &params[i];
        p->threadid = 0;
    }
    uint64_t sum = 0;
    for (int i = 0; i < THREAD_COUNT; i++) {
        test_thread_param_t* p = &params[i];
        pthread_join(p->thread, NULL);
        fprintf(stderr, "thread id %d: %llu messages\n", i + 1, p->messages);
        sum += p->messages;
    }
    fprintf(stdout, "%d threads sent a total of %llu messages\n", THREAD_COUNT, sum);

    unclog_close(logger);
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
    struct timespec start, stop, diff;
    (void)argc;
    (void)argv;

    unclog_init("[Defaults]\nDetails=Full\nLevel=Trace\nSinks=stderr\n[thread]\nLevel=Trace");

    if (argc >= 2) THREAD_COUNT = atoi(argv[1]);

    fprintf(stderr, "----------------------------------------------------------------------\n");

    test_thread_param_t* params = calloc(sizeof(test_thread_param_t), THREAD_COUNT);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < THREAD_COUNT; i++) {
        test_thread_param_t* p = &params[i];
        memcpy(p, &default_params, sizeof(test_thread_param_t));
        p->threadid = i + 1;
        pthread_create(&p->thread, NULL, test_thread, p);
    }

    pthread_t timerthread;
    pthread_create(&timerthread, NULL, sleepthread, params);
    pthread_join(timerthread, NULL);
    clock_gettime(CLOCK_MONOTONIC, &stop);

    fprintf(stderr, "----------------------------------------------------------------------\n");

    diff = timespec_diff(stop, start);
    fprintf(stderr, "start %ld.%09ld seconds\n", start.tv_sec, start.tv_nsec);
    fprintf(stderr, "stop %ld.%09ld seconds\n", stop.tv_sec, stop.tv_nsec);

    uint64_t sum = unclog_sink_stderr_get_num_messages();

    double msg_sec = sum / (diff.tv_sec + (diff.tv_nsec / 1000000000.0));
    char* postfix = "";
    if (msg_sec > 1000) {
        msg_sec /= 1000;
        postfix = "k";
    }
    if (msg_sec > 1000) {
        msg_sec /= 1000;
        postfix = "M";
    }

    fprintf(stdout, "%llu messages took %ld.%09ld seconds: %.3lf%s msgs/sec\n", sum, diff.tv_sec,
            diff.tv_nsec, msg_sec, postfix);

    fprintf(stderr, "----------------------------------------------------------------------\n");

    free(params);
    return 0;
}
