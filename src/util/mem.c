#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "arralloc.h"

#define FREE(ptr) \
({\
	if(ptr != NULL)\
	{\
		free(ptr);\
		ptr = NULL;\
	}\
})

buf_str allocate_serial_buffers(slc_str slice)
{
	buf_str buffer;

	buffer.master = NULL;
	buffer.local = NULL;
	buffer.old = NULL;
	buffer.new = NULL;
	buffer.edge = NULL;

	buffer.local = (double **) arralloc(sizeof(double), 2, slice.actual.width, slice.actual.height);
	buffer.old = (double **) arralloc(sizeof(double), 2, slice.halo.width, slice.halo.height);
	buffer.new = (double **) arralloc(sizeof(double), 2, slice.halo.width, slice.halo.height);
	buffer.edge = (double **) arralloc(sizeof(double), 2, slice.halo.width, slice.halo.height);

	return buffer;
}

buf_str allocate_parallel_buffers(comm_str comm, slc_str slice)
{
	buf_str buffer;

	buffer.master = NULL;
	buffer.local = NULL;
	buffer.old = NULL;
	buffer.new = NULL;
	buffer.edge = NULL;

	/* Allocate arrays dynamically using special malloc routine. */
	buffer.master = (double **) arralloc(sizeof(double), 2, slice.padded.width*comm.size, slice.padded.height);
	buffer.local  = (double **) arralloc(sizeof(double), 2, slice.padded.width, slice.padded.height);
	// sliced images with halo dimensions
	buffer.old  = (double **) arralloc(sizeof(double), 2, slice.halo.width, slice.halo.height);
	buffer.new  = (double **) arralloc(sizeof(double), 2, slice.halo.width, slice.halo.height);
	buffer.edge = (double **) arralloc(sizeof(double), 2, slice.halo.width, slice.halo.height);

	return buffer;
}

void dealocate_buffers(buf_str *buffers)
{
	FREE(buffers->master);
	FREE(buffers->local);
	FREE(buffers->old);
	FREE(buffers->new);
	FREE(buffers->edge);
}

int swap_ptrs(double ***ptr1, double ***ptr2)
{
	double **ptr_temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = ptr_temp;

	return 0;
}