#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>



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

int main() {
    /*long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cpus == -1) {
        fprintf(stderr, "sysconf failed: %s\n", strerror(errno));
        return 1;
    }
    printf("Number of CPUs: %ld\n", num_cpus);
    int core_id = 0;
    if (core_id < 0 || core_id >= num_cpus) {
        printf("Invalid core_id: %d (must be between 0 and %ld)\n", core_id, num_cpus - 1);
        return 1;
    }*/

    if (set_affinity(0)) {
        printf("Successfully set affinity to core %d\n", 0);
    } else {
        printf("Failed to set affinity\n");
    }
    return 0;
}
