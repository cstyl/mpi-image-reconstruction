#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "mplib.h"

#define TRUE  1
#define FALSE 0
#define ndims 2   
#define vertical    1 
#define horizontal  0

MPI_Datatype hor_halo_type;

void mpstart(comm_str *comm)
{ 
  MPI_Init(NULL, NULL);
  comm->comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm->comm, &comm->rank);
  MPI_Comm_size(comm->comm, &comm->size);
}
  
void mpstop(void)
{ 
  MPI_Finalize();
}

void setup_cartesian_topology(comm_str *comm, cart_str *cart)
{
	/* Setup cartesian topology */
	cart->dims[0] = 0;
	cart->dims[1] = 0;
	cart->period[0] = FALSE; 	// Acyclic to form a line in the horizontal
	cart->period[1] = TRUE; 	// Cyclic to form a ring in the vertical  
	cart->reorder = FALSE;    // Do not reorder
	cart->disp = 1;           // shift by 1

	/* Set the dimensions, create the 2D Cartesian topology 
	 * and get the rank of the current process
	 */
	MPI_Dims_create(comm->size, ndims, cart->dims);
	MPI_Cart_create(comm->comm, ndims, cart->dims, cart->period, cart->reorder, &cart->comm2d);

	MPI_Comm_rank(cart->comm2d, &comm->rank);
	MPI_Cart_coords(cart->comm2d, comm->rank, ndims, cart->coords);

	/* First compute the ranks of the neighbouring processes 
	 * along the horizontal axis (left to right) 
	 * and then along vertical axis (top to bottom)
	 */
	MPI_Cart_shift(cart->comm2d, horizontal, cart->disp, &cart->l.val, &cart->r.val);
	MPI_Cart_shift(cart->comm2d, vertical, cart->disp, &cart->b.val, &cart->t.val);
}

void send_halos_2D(cart_str *cart, MPI_Datatype hor_halo_type, img_str *img, slc_str slice)
{
	/*
	 * Use non-blocking communications.
	 * First use non-blocking asynchronous sent to sent the vertical halo values 
	 * from left to right and then from right to left
	 * Then sent the horisontal halo values 
	 * from top to bottom and then from bottom to top
	 */ 
    MPI_Issend(&img->buffers.old[slice.actual.width][1], slice.actual.height, MPI_DOUBLE, cart->r.val, 1, cart->comm2d, &cart->l.sreq);
    MPI_Issend(&img->buffers.old[1][1], slice.actual.height, MPI_DOUBLE, cart->l.val, 2, cart->comm2d, &cart->r.sreq);
	MPI_Issend(&img->buffers.old[1][1], 1, hor_halo_type, cart->b.val, 3, cart->comm2d, &cart->t.sreq);	
	MPI_Issend(&img->buffers.old[1][slice.actual.height], 1, hor_halo_type, cart->t.val, 4, cart->comm2d, &cart->b.sreq);

}		

void receive_halos_2D(cart_str *cart, MPI_Datatype hor_halo_type, img_str *img, slc_str slice)
{

	/* Use non-blocking asynchronous receive calls */
	MPI_Irecv(&img->buffers.old[0][1], slice.actual.height, MPI_DOUBLE, cart->l.val, 1, cart->comm2d, &cart->r.rreq);	// leftmost on halo
	MPI_Irecv(&img->buffers.old[slice.actual.width+1][1], slice.actual.height, MPI_DOUBLE, cart->r.val, 2, cart->comm2d, &cart->l.rreq); // rightmost on halo
	MPI_Irecv(&img->buffers.old[1][slice.actual.height+1], 1, hor_halo_type, cart->t.val, 3, cart->comm2d, &cart->b.rreq);	
	MPI_Irecv(&img->buffers.old[1][0], 1, hor_halo_type, cart->b.val, 4, cart->comm2d, &cart->t.rreq);
}

void complete_communication_2D(cart_str *cart)
{
	/*
	 * Wait for left to right and right to left sends to complete
	 * Wait for bottom to top and bottom to top sends to complete
	 */ 
	MPI_Wait(&cart->l.sreq, &cart->r.sstat);
	MPI_Wait(&cart->r.sreq, &cart->l.sstat);
	MPI_Wait(&cart->t.sreq, &cart->b.sstat);
	MPI_Wait(&cart->b.sreq, &cart->t.sstat);
	MPI_Wait(&cart->l.rreq, &cart->r.rstat);
	MPI_Wait(&cart->r.rreq, &cart->l.rstat);
	MPI_Wait(&cart->t.rreq, &cart->b.rstat);
	MPI_Wait(&cart->b.rreq, &cart->t.rstat);

}

void mpscatter(cart_str cart, double *inbuff, double *outbuff, int ndata)
{
	MPI_Scatter(inbuff,  ndata, MPI_DOUBLE, outbuff, ndata, MPI_DOUBLE, 0, cart.comm2d);
}

void mpgather(cart_str cart, double *inbuff, double *outbuff, int ndata)
{
	MPI_Gather(inbuff,  ndata, MPI_DOUBLE, outbuff, ndata, MPI_DOUBLE, 0, cart.comm2d);
}

double mpgmax(cart_str cart, double *local_max)
{
  double global_max;
 
  MPI_Allreduce(local_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, cart.comm2d);

  return global_max;
} 

double mpgsum(cart_str cart, double *local_sum)
{
  double global_sum;
 
  MPI_Allreduce(local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, cart.comm2d);

  return global_sum;
} 

double gettime(void)
{ 
  return MPI_Wtime(); 
} 