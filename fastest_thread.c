#define _GNU_SOURCE // For CPU_ZERO, CPU_SET, sched_setaffinity etc, needed for sched.h
#include <sched.h> // For sched_setaffinity, sched_getaffinity, cpu_set_t, etc
#include <stdio.h> // For printf
#include <errno.h> // For errno
#include <string.h> // For strerror
#include <unistd.h> // For sysconf
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

struct timespec ts;
int difficulty = 1000000;
const double PI = 3.1415;
int runtime = 0;


int set_affinity(int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    int ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    if (ret != 0) {
        fprintf(stderr, "sched_setaffinity failed: %s\n", strerror(errno));
        return 0;
    }
    return 1;
}


void run(int core_id) {

    clock_gettime(CLOCK_REALTIME, &ts);
    double startTime = ( (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9 );

    for (int i = 0; i < difficulty; i++) {
        int mao = (int)random() % (int)round(PI * 100000000);
        double mau = pow((double)(mao + 1) / 2 * 1.5000, 3);
        mau = sqrt(mau);
        mau = pow(mau, -4);
        mau = pow(mau, 3);
    }

    clock_gettime(CLOCK_REALTIME, &ts);
    double endTime = ( (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9 );
    printf("Core %d: \t", core_id);
    printf("%f seconds\n", endTime - startTime);
}

void* run_thread(void* args) {

    if (args != NULL) {
        run(*(int*)args);
    } else {
        printf("args = NULL\n");
    }

    return NULL;
}


pthread_t test_threaded(int *core_id_ptr, long num_cpus) {
    int core_id = *core_id_ptr;

    if (core_id < 0 || core_id >= num_cpus) {
        printf("Invalid core_id: %d (must be between 0 and %ld)\n", core_id, num_cpus - 1);
        return -1;
    }


    if (set_affinity(core_id)) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, run_thread, core_id_ptr);
        return thread_id;
    } else {
        printf("Failed to set affinity\n");
        return -1;
    }
}

void test_each_core(long num_cpus) {

    for (int core_id = 0; core_id < num_cpus; core_id++) {
        if (core_id < 0 || core_id >= num_cpus) {
            printf("Invalid core_id: %d (must be between 0 and %ld)\n", core_id, num_cpus - 1);
            return;
        }

        if (set_affinity(core_id)) {
            //run(core_id);
            // or

            pthread_join(test_threaded(&core_id, num_cpus), NULL);

        } else {
            printf("Failed to set affinity\n");
        }
    }
}




int main() {
    clock_gettime(CLOCK_REALTIME, &ts);
    double startTime = ( (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9 );


    long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cpus == -1) {
        fprintf(stderr, "sysconf failed: %s\n", strerror(errno));
        return 1;
    } else {
        printf("This system has %ld CPU cores\n", num_cpus);
    }

    printf("\nTesting each core with %d difficulty:\n", difficulty);
    test_each_core(num_cpus);

    printf("\nTesting hyperthread + multicore characteristics of the cpu:\n");
    int iterations = (num_cpus * (num_cpus - 1)) / 2;
    int current = 1;

    for (int i = 0; i < num_cpus; i++) {
        for (int j = i + 1; j < num_cpus; j++) {
            printf("Iteration %d of %d: %d with %d\n", current, iterations, i, j);
            pthread_t thread1 = test_threaded(&i, num_cpus);
            pthread_t thread2 = test_threaded(&j, num_cpus);

            if(thread1 != -1) pthread_join(thread1, NULL);
            if(thread2 != -1) pthread_join(thread2, NULL);

            printf("\n");
        }
    }


    clock_gettime(CLOCK_REALTIME, &ts);
    double endTime = ( (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9 );
    printf("Test took %lf seconds from start to finish\n", endTime - startTime);
    return 0;
}
