#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cpu_usage.h"

#define TICKS_PER_SECOND 100
#define TICKS_PER_UPDATE (TICKS_PER_SECOND / CPU_UPDATES_PER_SECOND)
#define NUM_PACKAGES 2
#define NUM_CORES 2

static unsigned int busy[NUM_CORES];
static unsigned int total[NUM_CORES];

int get_num_packages(void) {
    unsigned int num_packages = NUM_PACKAGES;

    srand(time(0));

    return num_packages;
}

int fetch_ticks_by_core(unsigned int **ticks_by_core_busy_ptr, unsigned int **ticks_by_core_total_ptr, unsigned int *num_cores_ptr) {
    unsigned int num_cores = NUM_CORES;

    unsigned int *ticks_by_core_busy = calloc(num_cores, sizeof(unsigned int));
    unsigned int *ticks_by_core_total = calloc(num_cores, sizeof(unsigned int));

    if (ticks_by_core_busy == NULL || ticks_by_core_total == NULL) {
        printf("Failed to alloc\n");
        return -1;
    }

    for (size_t core = 0; core < num_cores; core++) {
        busy[core] += rand() % TICKS_PER_UPDATE;
        total[core] += TICKS_PER_UPDATE;
        ticks_by_core_busy[core] = busy[core];
        ticks_by_core_total[core] = total[core];
    }

    *ticks_by_core_busy_ptr = ticks_by_core_busy;
    *ticks_by_core_total_ptr = ticks_by_core_total;
    *num_cores_ptr = num_cores;

    return 0;
}
