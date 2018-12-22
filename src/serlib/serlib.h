#ifndef __SERLIB_H__
#define __SERLIB_H__

#include "structs.h"

void serial_read_args(int argc, char **argv, master_str *master);
void serial_read_data(master_str *master);
void serial_initialise_buffers(master_str *master);

void serial_process(master_str *master);
void serial_image_update(master_str *master);
double serial_max_pixel_difference(master_str master);
double serial_global_pixel_average(master_str master);

double serial_gettime(void);
void serial_start_timing(master_str *master);
void serial_stop_timing(master_str *master);
void serial_print_timing(master_str master);

void serial_write_data(master_str *master);
void serial_clean_buffers(master_str *master);

#endif //__SERLIB_H__