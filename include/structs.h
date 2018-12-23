#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <mpi.h>
#define ndims 2 

typedef enum version_enum
{
	serial,
	par2D
}version;

typedef struct dimensions_struct
{
	int width;
	int height;
} dim_str;

typedef struct buf_struct
{
	double **master;
	double **local;
	double **new;
	double **old;
	double **edge;
}buf_str;

/* structure containing all the neccessary 
 * information about the image
 */
typedef struct image_struct
{
	buf_str buffers;
	dim_str size;

	char efilename[FILENAME_MAX];
	char ifilename[FILENAME_MAX];
} img_str;
/* structure containing all the neccessary 
 * information about the sliced image
 */
typedef struct slice_struct
{
	dim_str padded;
	dim_str actual;
	dim_str rem;
	dim_str halo;
	dim_str ustart,uend;
}slc_str;

typedef struct parameters_struct
{
	int version;
	int iterations;
	int delta_freq;
	int avg_freq;
	int citer;
	double avg_pixel;
}params_str;

/* Contains the inforamtion about a neighbour */
typedef struct dim_comm_struct
{
	int val;
	MPI_Status sstat,rstat;
	MPI_Request sreq,rreq;
}dir_str;

/* Variables for 2D Cartesian topology */
typedef struct cartesian_struct
{
	MPI_Comm comm2d;
	int disp;
	int dims[ndims];
	int period[ndims];
	int coords[ndims];
	int reorder;
	dir_str r, l, b, t;
} cart_str;

/* Communication variables */
typedef struct communication_struct
{
	MPI_Comm comm;
	int rank, size;
	dir_str r, l;
}comm_str;

typedef struct time_struct
{
	double start;
	double local;
	double average;
}time_str;

typedef struct master_struct
{
	img_str img;
	slc_str slice;
	params_str params;

	comm_str comm;
	cart_str cart;

	time_str time;
}master_str;

#endif	//__STRUCTS_H__