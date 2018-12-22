#ifndef __IMGLIB_H__
#define __IMGLIB_H__

#include "structs.h"

slc_str get_serial_dims(img_str img);
slc_str get_parallel_dims(img_str img, cart_str cart);

void initialise_edges(double **edge, double **buf, dim_str slice);
void initialise_img_buffers(double **old, double **new, double val, dim_str halo);

void update_buffer(double **buf, double **old, dim_str slice);
void update_image_slice(img_str *img, dim_str begin, dim_str end);
void update_image_slice_boundaries(img_str *img, dim_str slice);

void set_left_boundary(img_str *img, slc_str slice, int yi, double val);
void set_right_boundary(img_str *img, slc_str slice, int yi, double val);
void set_top_boundary(img_str *img, int xi);
void set_bottom_boundary(img_str *img, int xi);

void setup_periodic_boundaries(img_str *img);
void setup_sawtooth_boundaries(img_str *img, slc_str slice);
void setup_parallel_sawtooth_boundaries(cart_str cart, img_str *img, slc_str slice);

double boundaryval(int i, int m);

double get_local_maximum(img_str img, dim_str slice);
double get_local_sum(img_str img, dim_str slice);
#endif	//__IMGLIB_H__