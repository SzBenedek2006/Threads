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
#include <stdbool.h>

struct timespec ts;
int difficulty = 1000000;
const double PI = 3.1415;
int runtime = 0;

//flags
bool fHelp = false;
bool fExtended = false;



int set_flags(int argc, char* argv[]) { // do it in flag order, later ones can override previous ones like with rm command
    for (int i = 1; i < argc; i++) { // iterate args
        //printf("%s\n", argv[i]);


        if (argv[i][0] == '-') { // Handle flag
            //printf("%s\n", argv[i]);
            if (argv[i][1] != '-') { // short flags
                for (int j = 1; j < strlen(argv[i]); j++) { // iterate one arg
                    //printf("%c\n", argv[i][j]);
                    switch (argv[i][j]) {
                        case 'h':
                            fHelp = true;
                            break;
                        case 'e':
                            fExtended = true;
                            break;
                        default:
                            printf("Unknown argument\n");
                    }
                }
            } else { // long flags
                if (strcmp(argv[i], "--difficulty") == 0 || strcmp(argv[i], "--diff") == 0) {
                    i++;
                    if (argv[i] != NULL) { // 9 character long is safe
                        if (strlen(argv[i]) >= 10) {
                            printf("Max value is 999999999 (9 characters). Exiting!\n");
                            return 0;
                        } else {
                            difficulty = atoi(argv[i]); // Very not NULL safe
                        }
                    } else {
                        printf("Missing difficulty parameter. Exiting!\n");
                        return 0;
                    }
                }
            }
        }
        else {
            printf("Found garbage data in flags, continuing...\n");
        }
    }
    return 1;

}






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




int main(int argc, char* argv[]) {
    clock_gettime(CLOCK_REALTIME, &ts);
    double startTime = ( (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9 );

    if (!set_flags(argc, argv)) {
        return 1;
    }
    if (fHelp) {
        //TODO: implement help message
        return 0;
    }

    long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cpus == -1) {
        fprintf(stderr, "sysconf failed: %s\n", strerror(errno));
        return 1;
    } else {
        printf("This system has %ld CPU cores\n", num_cpus);
    }

    printf("\nTesting each core with %d difficulty:\n", difficulty);
    test_each_core(num_cpus);

    if (fExtended) {
        printf("\nTesting hyperthread + multicore characteristics of the cpu:\n");
            int iterations = (num_cpus * (num_cpus - 1)) / 2;
            int current = 1;

            for (int i = 0; i < num_cpus; i++) {
                for (int j = i + 1; j < num_cpus; j++) {
                    printf("Iteration %d of %d: %d with %d\n", current++, iterations, i, j);
                    pthread_t thread1 = test_threaded(&i, num_cpus);
                    pthread_t thread2 = test_threaded(&j, num_cpus);

                    if(thread1 != -1) pthread_join(thread1, NULL);
                    if(thread2 != -1) pthread_join(thread2, NULL);

                    printf("\n");
                }
            }
    }



    clock_gettime(CLOCK_REALTIME, &ts);
    double endTime = ( (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9 );
    printf("Test took %lf seconds from start to finish\n", endTime - startTime);
    return 0;
}
