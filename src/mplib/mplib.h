#ifndef __MPLIB_H__
#define __MPLIB_H__

#include "structs.h"

void mpstart(comm_str *comm);
void mpstop(void);

void setup_cartesian_topology(comm_str *comm, cart_str *cart);

void send_halos_2D(cart_str *cart, MPI_Datatype hor_halo_type, img_str *img, slc_str slice);
void receive_halos_2D(cart_str *cart, MPI_Datatype hor_halo_type, img_str *img, slc_str slice);
void complete_communication_2D(cart_str *cart);

void mpscatter(cart_str cart, double *inbuff, double *outbuff, int ndata);
void mpgather(cart_str, double *inbuff, double *outbuff, int ndata);

double mpgmax(cart_str cart, double *local_max);
double mpgsum(cart_str cart, double *local_sum);

double gettime(void);

#endif	//__MPLIB_H__