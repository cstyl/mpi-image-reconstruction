#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "parlib.h"
#include "args.h"
#include "mem.h"
#include "pgmio.h"
#include "imglib.h"
#include "mplib.h"

#define PRINTFREQ 200
#define MASTER_PROCESS 0
#define STOP_DELTA 0.1
#define TRUE  1
#define FALSE 0

void par_initialise_comm(master_str *master)
{
	mpstart(&master->comm);
	setup_cartesian_topology(&master->comm, &master->cart);
}

void par_read_args(int argc, char **argv, master_str *master)
{
	read_parameters(argc, argv, &master->params);

	if(master->comm.rank == MASTER_PROCESS)
	{
		printf("Running on %d processes.\n", master->comm.size);
		printf("Selected topology: [ %d , %d ]\n", master->cart.dims[0], master->cart.dims[1]);	
		// read command line arguments and get the image size
		read_filenames(argc, argv, &master->img, par2D);
		pgmsize(master->img.efilename, &master->img.size.width, &master->img.size.height);
		print_params_filenames(master->img, master->params);
	}
}

void par_read_scatter_data(master_str *master)
{
	// broadcast image size to all processes
	MPI_Bcast(&master->img.size, 2, MPI_INT, MASTER_PROCESS, master->cart.comm2d);
	master->slice = get_parallel_dims(master->img, master->cart);
	
	master->img.buffers = allocate_parallel_buffers(master->comm, master->slice);

	// read image on master process
	if(master->comm.rank == MASTER_PROCESS)
    {
		printf("Processing %d x %d image on %d processes\n", 
				master->img.size.width, master->img.size.height, master->comm.size);
		printf("Reading <%s>\n\n", master->img.efilename);
	    pgmread_generalised_cascaded(master->img.efilename, &master->img.buffers.master[0][0], 
	    							 master->cart, master->img.size, master->slice);
	    printf("Scattering data...");
    }

    mpscatter(master->cart, &master->img.buffers.master[0][0], &master->img.buffers.local[0][0], 
    		  master->slice.padded.width * master->slice.padded.height);

	if(master->comm.rank == MASTER_PROCESS)
    {
	    printf("\tDone\n");
    }
}

void par_initialise_buffers(master_str *master)
{
	/* Initialise edge slices to buf values 
	 * old and new image arrays to 255(white)
	 * and setup fixed sawtooth boundaries to left and right
	 */
	initialise_edges(master->img.buffers.edge, master->img.buffers.local, master->slice.actual);
	initialise_img_buffers(master->img.buffers.old, master->img.buffers.new, 255.0, master->slice.halo);
	setup_parallel_sawtooth_boundaries(master->cart, &master->img, master->slice);
}

void par_get_ustart_uend(master_str *master)
{
#ifdef OVERLAP
	master->slice.ustart.width  = 2;
	master->slice.ustart.height = 2;
	master->slice.uend.width  = master->slice.actual.width;
	master->slice.uend.height = master->slice.actual.height;
#else
	master->slice.ustart.width  = 1;
	master->slice.ustart.height = 1;
	master->slice.uend.width  = master->slice.actual.width  + 1;
	master->slice.uend.height = master->slice.actual.height + 1;
#endif	
}

void par_halo_exchange_image_update(master_str *master, MPI_Datatype hor_halo_type)
{
	par_get_ustart_uend(master);

    send_halos_2D(&master->cart, hor_halo_type, &master->img, master->slice);
    receive_halos_2D(&master->cart, hor_halo_type, &master->img, master->slice);

#ifdef OVERLAP
    /* update only the entries not on the boundaries until communication is complete*/
    update_image_slice(&master->img, master->slice.ustart, master->slice.uend);
#endif       

    complete_communication_2D(&master->cart);

#ifdef OVERLAP	
	/* update the boundaries received*/	
	update_image_slice_boundaries(&master->img, master->slice.actual);
#else
	update_image_slice(&master->img, master->slice.ustart, master->slice.uendm);
#endif
}

double par_max_pixel_difference(master_str master)
{
	double local_max_delta, max_delta=1.0;
	/*
	 * Determine stopping criterion: Perform global reduction 
	 * to find the maximum change across the entire image
	 * Check every DELTAFREQ iterations
	 */
	if(master.params.delta_freq!=0 && master.params.citer%master.params.delta_freq==0)
	{
		local_max_delta = get_local_maximum(master.img, master.slice.actual);
		max_delta = mpgmax(master.cart, &local_max_delta);
	}

	return max_delta;
}

double par_global_pixel_average(master_str master)
{
	double local_sum, global_avg=0.0;	
	/*
	 * Average pixel values in the image: Perform global reduction 
	 * to find the global sum of the pixels in the entire image
	 * Check every AVGFREQ iterations
	 */
	if(master.params.avg_freq!=0 && master.params.citer%master.params.avg_freq==0)
	{
		local_sum = get_local_sum(master.img, master.slice.actual);
		global_avg = mpgsum(master.cart, &local_sum)/(master.img.size.height * master.img.size.width);
		if(master.comm.rank == MASTER_PROCESS)
		{
			printf("Global Pixel Average = %.2f\n", global_avg);
		}
	}

	return global_avg;
}

void par_process(master_str *master)
{
	master->params.avg_pixel = 0.0;	
	double max_delta=1.0;

	MPI_Datatype hor_halo_type;
	MPI_Type_vector(master->slice.actual.width, 1, master->slice.halo.height, MPI_DOUBLE, &hor_halo_type);
	MPI_Type_commit(&hor_halo_type);

	master->params.citer=0;

    while((master->params.citer<master->params.iterations) && (max_delta > STOP_DELTA))
    {
    	master->params.citer++;
        if(master->params.citer%PRINTFREQ==0)
        {
            if(master->comm.rank == MASTER_PROCESS) printf("Iteration %d\n", master->params.citer);
        }

		par_halo_exchange_image_update(master, hor_halo_type);

		max_delta = par_max_pixel_difference(*master);

		master->params.avg_pixel = par_global_pixel_average(*master);
        /* Swap array pointers to avoid copying data from new to old */
        swap_ptrs(&master->img.buffers.old, &master->img.buffers.new);
    }
}

void par_start_timing(master_str *master)
{
#ifdef TIME
    MPI_Barrier(master->cart.comm2d);
    master->time.start = gettime();
#endif
}

void par_stop_timing(master_str *master)
{
#ifdef TIME
    MPI_Barrier(master->cart.comm2d);
    master->time.local = gettime() - master->time.start;
    master->time.average = mpgsum(master->cart, &master->time.local) / master->comm.size;
#endif	
}

void par_print_timing(master_str master)
{
#ifdef TIME
    printf("Average Time for %d iterations = %f\n", master.params.citer, master.time.average);
#endif	
}


void par_gather_write_data(master_str *master)
{
    update_buffer(master->img.buffers.local, master->img.buffers.old, master->slice.actual);

  	if(master->comm.rank == MASTER_PROCESS)
  	{
  		printf("\nFinished %d iterations\nGathering data...", master->params.citer);
	}

	/* gather the results from all slices into master buffer */
    mpgather(master->cart, &master->img.buffers.local[0][0], &master->img.buffers.master[0][0], 
    		 master->slice.padded.width * master->slice.padded.height);

  	if(master->comm.rank == MASTER_PROCESS)
  	{
		printf("\t Done\nWriting <%s>\n", master->img.ifilename); 
	    pgmwrite_generalised_cascaded(master->img.ifilename, &master->img.buffers.master[0][0], master->cart, master->img.size, master->slice);
	    par_print_timing(*master);
	}
}

void par_clean_buffers_stop_comm(master_str *master)
{
	dealocate_buffers(&master->img.buffers);
	mpstop();
}