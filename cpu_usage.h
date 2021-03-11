#pragma once

#define CPU_UPDATES_PER_SECOND 5
#define CPU_UPDATE_INTERVAL ((useconds_t)(1e6 / CPU_UPDATES_PER_SECOND))

int get_num_packages(void);
int fetch_ticks_by_core(unsigned int **ticks_by_core_busy_ptr, unsigned int **ticks_by_core_total_ptr, unsigned int *num_cores_ptr);

int cpu_usage_setup(void);
void cpu_update_usage_loop(volatile float *usage_breakdowns, unsigned int num_breakdowns);

