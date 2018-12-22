#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "serlib.h"
#include "args.h"
#include "mem.h"
#include "pgmio.h"
#include "imglib.h"

#define PRINTFREQ 200
#define STOP_DELTA 0.1

void serial_read_args(int argc, char **argv, master_str *master)
{
	read_parameters(argc, argv, &master->params);
    read_filenames(argc, argv, &master->img, serial);

    pgmsize(master->img.efilename, &master->img.size.width, &master->img.size.height);
    
    print_params_filenames(master->img, master->params);
}

void serial_read_data(master_str *master)
{
    printf("Processing %d x %d image\n", master->img.size.width, master->img.size.height);

    master->slice = get_serial_dims(master->img);

    master->img.buffers = allocate_serial_buffers(master->slice);
    printf("\nReading <%s>\n", master->img.efilename);
    pgmread(master->img.efilename, &master->img.buffers.local[0][0], 
    		master->slice.actual.width, master->slice.actual.height);
    printf("\n");
}

void serial_initialise_buffers(master_str *master)
{
	/* Initialise edge slices to buf values 
	 * old and new image arrays to 255(white)
	 * and setup fixed sawtooth boundaries to left and right
	 */
	initialise_edges(master->img.buffers.edge, master->img.buffers.local, master->slice.actual);
	initialise_img_buffers(master->img.buffers.old, master->img.buffers.new, 255.0, master->slice.halo);
    setup_sawtooth_boundaries(&master->img, master->slice);
}

void serial_process(master_str *master)
{
	master->params.avg_pixel = 0.0;	
	double max_delta=1.0;

	master->params.citer=0;

    while((master->params.citer<master->params.iterations) && (max_delta > STOP_DELTA))
    {
    	master->params.citer++;
        if(master->params.citer%PRINTFREQ==0)
        {
            printf("Iteration %d\n", master->params.citer);
        }

		serial_image_update(master);

		max_delta = serial_max_pixel_difference(*master);

		master->params.avg_pixel = serial_global_pixel_average(*master);
        /* Swap array pointers to avoid copying data from new to old */
        swap_ptrs(&master->img.buffers.old, &master->img.buffers.new);
    }
}

void serial_image_update(master_str *master)
{
	master->slice.ustart.width  = 1;
	master->slice.ustart.height = 1;
	master->slice.uend.width  = master->slice.actual.width  + 1;
	master->slice.uend.height = master->slice.actual.height + 1;

    /* Implement periodic boundary conditions on bottom and top sides */
    setup_periodic_boundaries(&master->img);

    update_image_slice(&master->img, master->slice.ustart, master->slice.uend);
}

double serial_max_pixel_difference(master_str master)
{
	double max_delta=1.0;
	/*
	 * Determine stopping criterion: Perform global reduction 
	 * to find the maximum change across the entire image
	 * Check every DELTAFREQ iterations
	 */
	if(master.params.delta_freq!=0 && master.params.citer%master.params.delta_freq==0)
	{
		max_delta = get_local_maximum(master.img, master.slice.actual);
	}

	return max_delta;
}

double serial_global_pixel_average(master_str master)
{
	double pixel_avg=0.0;	
	/*
	 * Average pixel values in the image: Perform global reduction 
	 * to find the global sum of the pixels in the entire image
	 * Check every AVGFREQ iterations
	 */
	if(master.params.avg_freq!=0 && master.params.citer%master.params.avg_freq==0)
	{
		pixel_avg = get_local_sum(master.img, master.slice.actual)/
					(master.img.size.height * master.img.size.width);
		printf("Pixel Average = %.2f\n", pixel_avg);
	}

	return pixel_avg;
}

double serial_gettime(void)
{ 
  	return MPI_Wtime(); 
} 

void serial_start_timing(master_str *master)
{
#ifdef TIME
    master->time.start = serial_gettime();
#endif
}

void serial_stop_timing(master_str *master)
{
#ifdef TIME
    master->time.average = serial_gettime() - master->time.start;
#endif	
}

void serial_print_timing(master_str master)
{
#ifdef TIME
    printf("Average Time for %d iterations = %f\n", master.params.citer, master.time.average);
#endif	
}


void serial_write_data(master_str *master)
{
    update_buffer(master->img.buffers.local, master->img.buffers.old, master->slice.actual);

    printf("\nWriting <%s>\n", master->img.ifilename); 
    pgmwrite(master->img.ifilename, &master->img.buffers.local[0][0], master->img.size.width, 
    		 master->img.size.height);
	serial_print_timing(*master);
}

void serial_clean_buffers(master_str *master)
{
	dealocate_buffers(&master->img.buffers);
}