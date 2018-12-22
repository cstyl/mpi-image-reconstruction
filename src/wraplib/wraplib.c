#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "wraplib.h"
#include "serlib.h"
#include "parlib.h"

int get_version(int an, char **av)
{
	int ai=1;
	int version=serial;	//default
	while(ai<an)
	{
		if(!strcmp(av[ai],"-ver"))
		{	
			if(atoi(av[ai+1])==0)
			{
				version = serial;
			}else
			{
				version = par2D;
			}
		}
		ai+=1;
	}

	return version;	
}

void setup_comm(master_str *master)
{
	if(master->params.version == par2D)
	{
		par_initialise_comm(master);
	}
}

void read_args(int an, char **av, master_str *master)
{
	if(master->params.version == serial)
	{
		serial_read_args(an,av,master);
	}else if(master->params.version == par2D)
	{
		par_read_args(an,av,master);
	}
}

void read_distribute_data(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_read_data(master);
	}else if(master->params.version == par2D)
	{
		par_read_scatter_data(master);
	}
}

void setup_buffers(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_initialise_buffers(master);
	}else if(master->params.version == par2D)
	{
		par_initialise_buffers(master);
	}
}

void start_timing(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_start_timing(master);
	}else if(master->params.version == par2D)
	{
		par_start_timing(master);
	}
}

void stop_timing(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_stop_timing(master);
	}else if(master->params.version == par2D)
	{
		par_stop_timing(master);
	}
}

void process(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_process(master);
	}else if(master->params.version == par2D)
	{
		par_process(master);
	}
}

void gather_write_data(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_write_data(master);
	}else if(master->params.version == par2D)
	{
		par_gather_write_data(master);
	}
}

void clean_buffers_end_comm(master_str *master)
{
	if(master->params.version == serial)
	{
		serial_clean_buffers(master);
	}else if(master->params.version == par2D)
	{
		par_clean_buffers_stop_comm(master);
	}
}