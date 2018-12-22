#ifndef __WRAPLIB_H__
#define __WRAPLIB_H__

#include "structs.h"

int get_version(int an, char **av);
void setup_comm(master_str *master);
void read_args(int an, char **av, master_str *master);
void read_distribute_data(master_str *master);
void setup_buffers(master_str *master);

void start_timing(master_str *master);
void stop_timing(master_str *master);
void process(master_str *master);

void gather_write_data(master_str *master);
void clean_buffers_end_comm(master_str *master);

#endif //__WRAPLIB_H__