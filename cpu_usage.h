#pragma once

int get_num_packages(void);
int fetch_ticks_by_core(unsigned int **ticks_by_core_busy_ptr, unsigned int **ticks_by_core_total_ptr, unsigned int *num_cores_ptr);

int cpu_usage_setup(void);
void cpu_update_usage_loop(volatile float *usage_breakdowns, unsigned int num_breakdowns);

