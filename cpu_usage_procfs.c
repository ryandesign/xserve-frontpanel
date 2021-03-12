#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

int get_num_packages(void) {
    FILE *fp;
    char buf[8];
    int last_package_id, num_packages = 0;
    bool found_one = false;

    if ((fp = popen("lscpu -p=SOCKET", "r")) == NULL) {
        printf("ERROR opening lscpu pipe: %s\n", strerror(errno));
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if (sscanf(buf, "%d", &last_package_id) == 1) {
            found_one = true;
        }
    }

    if (found_one) {
        num_packages = last_package_id + 1;
    }

    if (pclose(fp))  {
        printf("ERROR running lscpu: %s\n", strerror(errno));
        return -1;
    }

    if (num_packages <= 0) {
        printf("WARNING: lscpu gave invalid number of packages %d, using 1\n", num_packages);
        num_packages = 1;
    }

    return num_packages;
}

int fetch_ticks_by_core(unsigned int **ticks_by_core_busy_ptr, unsigned int **ticks_by_core_total_ptr, unsigned int *num_cores_ptr) {
    int num_cores = get_nprocs_conf();
    FILE *fp;
    char buf[100];
    unsigned long user, nice, system, idle, busy;
    size_t core;

    // Aggregate per-core ticks into busy & total counts
    unsigned int *ticks_by_core_busy = calloc(num_cores, sizeof(unsigned int));
    unsigned int *ticks_by_core_total = calloc(num_cores, sizeof(unsigned int));

    if (ticks_by_core_busy == NULL || ticks_by_core_total == NULL) {
        printf("Failed to alloc\n");
        return -1;
    }

    if ((fp = fopen("/proc/stat", "r")) == NULL) {
        printf("ERROR opening /proc/stat: %s\n", strerror(errno));
        return -1;
    }

    core = 0;
    while (core < num_cores && fgets(buf, sizeof(buf), fp) != NULL) {
        if (buf[0] == 'c' && buf[1] == 'p' && buf[2] == 'u' && buf[3] >= '0' && buf[3] <= '9') {
            if (sscanf(buf, "%*s %ld %ld %ld %ld", &user, &nice, &system, &idle) == 4) {
                ticks_by_core_busy[core] = busy = user + nice + system;
                ticks_by_core_total[core] = busy + idle;
                core++;
            }
        }
    }

    if (fclose(fp)) {
        printf("ERROR closing /proc/stat: %s\n", strerror(errno));
        return -1;
    }

    // Output aggregated data
    *ticks_by_core_busy_ptr = ticks_by_core_busy;
    *ticks_by_core_total_ptr = ticks_by_core_total;
    *num_cores_ptr = num_cores;

    return 0;
}
