#ifndef __PARLIB_H__
#define __PARLIB_H__

#include "structs.h"

void par_initialise_comm(master_str *master);
void par_read_args(int argc, char **argv, master_str *master);
void par_read_scatter_data(master_str *master);
void par_initialise_buffers(master_str *master);

void par_process(master_str *master);
void par_halo_exchange_image_update(master_str *master, MPI_Datatype hor_halo_type);
void par_get_ustart_uend(master_str *master);
double par_max_pixel_difference(master_str master);
double par_global_pixel_average(master_str master);

void par_start_timing(master_str *master);
void par_stop_timing(master_str *master);
void par_print_timing(master_str master);

void par_gather_write_data(master_str *master);
void par_clean_buffers_stop_comm(master_str *master);
#endif	//__PARLIB_H__