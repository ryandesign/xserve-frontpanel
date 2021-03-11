#include <errno.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

int get_num_packages(void) {
    int num_packages;
    size_t sizeof_num_packages = sizeof(num_packages);

    if (sysctlbyname("hw.packages", &num_packages, &sizeof_num_packages, NULL, 0) != 0) {
        printf("ERROR fetching hw.packages: %s\n", strerror(errno));
        return -1;
    }

    if (num_packages <= 0) {
        printf("WARNING: hw.packages gave invalid number %d, using 1\n", num_packages);
        num_packages = 1;
    }

    return num_packages;
}

int fetch_ticks_by_core(unsigned int **ticks_by_core_busy_ptr, unsigned int **ticks_by_core_total_ptr, unsigned int *num_cores_ptr) {
    // Fetch load info
    natural_t num_cores;
    processor_cpu_load_info_data_t* load_info;
    mach_msg_type_number_t load_info_len;

    kern_return_t err = host_processor_info(mach_host_self(),
                                            PROCESSOR_CPU_LOAD_INFO,
                                            &num_cores,
                                            (processor_info_array_t *)&load_info,
                                            &load_info_len);
    if (err != KERN_SUCCESS) {
        printf("ERROR fetching CPU load info: %d\n", err);
        return -1;
    }

    // Aggregate per-core ticks into busy & total counts
    unsigned int *ticks_by_core_busy = calloc(num_cores, sizeof(unsigned int));
    unsigned int *ticks_by_core_total = calloc(num_cores, sizeof(unsigned int));

    if (ticks_by_core_busy == NULL || ticks_by_core_total == NULL) {
        printf("Failed to alloc\n");
        return -1;
    }

    for (size_t core = 0; core < num_cores; core++) {
        unsigned int *ticks_by_state = load_info[core].cpu_ticks;
        for (int state = 0; state < CPU_STATE_MAX; state++) {
            ticks_by_core_total[core] += ticks_by_state[state];
            if (state != CPU_STATE_IDLE) {
                ticks_by_core_busy[core] += ticks_by_state[state];
            }

        }
    }

    // Deallocate memory from the host_processor_info call
    vm_deallocate(mach_task_self(), (vm_address_t)load_info, load_info_len);

    // Output aggregated data
    *ticks_by_core_busy_ptr = ticks_by_core_busy;
    *ticks_by_core_total_ptr = ticks_by_core_total;
    *num_cores_ptr = num_cores;

    return 0;
}
