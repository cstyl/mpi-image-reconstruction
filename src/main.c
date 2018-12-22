#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "wraplib.h"

int main(int argc, char **argv)
{
	master_str master;

	master.params.version = get_version(argc,argv);

	setup_comm(&master);

	read_args(argc, argv, &master);

	read_distribute_data(&master);

	setup_buffers(&master);

	start_timing(&master);

	process(&master);

	stop_timing(&master);

	gather_write_data(&master);

	clean_buffers_end_comm(&master);

	return 0;
}