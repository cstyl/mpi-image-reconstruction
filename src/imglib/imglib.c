#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imglib.h"

slc_str get_serial_dims(img_str img)
{
    slc_str slice;

    slice.actual.width = img.size.width;
    slice.actual.height = img.size.height;

    slice.rem.width = 0;
    slice.rem.height = 0;

    slice.padded.width  = slice.actual.width  + slice.rem.width;
    slice.padded.height = slice.actual.height + slice.rem.height;

    slice.halo.width = slice.actual.width + 2;
    slice.halo.height = slice.actual.height + 2;

    return slice;
}

slc_str get_parallel_dims(img_str img, cart_str cart)
{
    slc_str slice;
    /* Get the actual dimensions of each 2D slice */
    slice.actual.width  = (int) floor((double)img.size.width  / (double)cart.dims[0]);
    slice.actual.height = (int) floor((double)img.size.height / (double)cart.dims[1]);
    /* If not exactly divisible with the nubmer of processes, 
     * boundary slices will have extra elements
     */
    slice.rem.width  = img.size.width  - slice.actual.width  * cart.dims[0];
    slice.rem.height = img.size.height - slice.actual.height * cart.dims[1];
    /* Get the padded dimensions of each 2D slice */
    slice.padded.width  = slice.actual.width  + slice.rem.width;
    slice.padded.height = slice.actual.height + slice.rem.height;

    /* if on the right boundary of the topology recalculate slice width
     * to consider extra pixels
     * if number of processes not exactly divisible with horizontal pixels
     * this should be different to the padded width
     */
    if(cart.coords[0] == cart.dims[0] - 1)
        slice.actual.width = slice.padded.width;

    /* if on the top boundary of the topology recalculate slice height
     * to consider extra pixels
     * if number of processes not exactly divisible with vertical pixels
     * this should be different to the padded height
     */
    if(cart.coords[1] == cart.dims[1] - 1)
        slice.actual.height = slice.padded.height;

    slice.halo.height = slice.actual.height + 2;
    slice.halo.width  = slice.actual.width  + 2;

    return slice;
}

void initialise_edges(double **edge, double **buf, dim_str slice)
{
	int xi,yi;

    for(xi=1; xi<slice.width+1; xi++)
    {
        for(yi=1; yi<slice.height+1; yi++)
        {
        	edge[xi][yi] = buf[xi-1][yi-1];
        }
    }
}

void initialise_img_buffers(double **old, double **new, double val, dim_str halo)
{
	int xi,yi;

    for(xi=0; xi<halo.width; xi++)
    {
        for(yi=0; yi<halo.height; yi++)
        {
            old[xi][yi] = val;
            new[xi][yi] = val;
        }
    }
}

void update_buffer(double **buf, double **old, dim_str slice)
{
    int xi,yi;

    for(xi=1; xi<slice.width+1; xi++)
    {
        for(yi=1; yi<slice.height+1; yi++)
        {
            buf[xi-1][yi-1] = old[xi][yi];
        }
    }
}

void update_image_slice(img_str *img, dim_str begin, dim_str end)
{
    int xi, yi;

    for(xi=begin.width; xi<end.width; xi++)
    {
        for(yi=begin.height; yi<end.height; yi++)
        {
            img->buffers.new[xi][yi] = 0.25 * (img->buffers.old[xi-1][yi]     // left neighbour
                                                + img->buffers.old[xi+1][yi]   // right neighbour 
                                                + img->buffers.old[xi][yi-1]   // bottom neighbour 
                                                + img->buffers.old[xi][yi+1]   // top neighbour 
                                                - img->buffers.edge[xi][yi]
                                                );
        }
    }
}

void update_image_slice_boundaries(img_str *img, dim_str slice)
{
    int xi, yi;

    // update horizontal boundaries
    for(xi=1; xi<slice.width+1; xi++)
    {
        // update bottom
        img->buffers.new[xi][1] = 0.25 * (img->buffers.old[xi-1][1]       // left neighbour
                                            + img->buffers.old[xi+1][1]   // right neighbour 
                                            + img->buffers.old[xi][0]     // bottom neighbour 
                                            + img->buffers.old[xi][2]     // top neighbour 
                                            - img->buffers.edge[xi][1]
                                        );
        // update top
        img->buffers.new[xi][slice.height] = 0.25 * (img->buffers.old[xi-1][slice.height]     // left neighbour
                                                    + img->buffers.old[xi+1][slice.height]   // right neighbour 
                                                    + img->buffers.old[xi][slice.height-1]   // bottom neighbour 
                                                    + img->buffers.old[xi][slice.height+1]   // top neighbour 
                                                    - img->buffers.edge[xi][slice.height]
                                                    );
    }

    // update vertical boundaries
    for(yi=1; yi<slice.height+1; yi++)
    {
        // update leftmost
        img->buffers.new[1][yi] = 0.25 * (img->buffers.old[0][yi]         // left neighbour
                                            + img->buffers.old[2][yi]     // right neighbour 
                                            + img->buffers.old[1][yi-1]   // bottom neighbour 
                                            + img->buffers.old[1][yi+1]   // top neighbour 
                                            - img->buffers.edge[1][yi]
                                            );
        // update rightmost
        img->buffers.new[slice.width][yi] = 0.25 * (img->buffers.old[slice.width-1][yi]     // left neighbour
                                                    + img->buffers.old[slice.width+1][yi]   // right neighbour 
                                                    + img->buffers.old[slice.width][yi-1]   // bottom neighbour 
                                                    + img->buffers.old[slice.width][yi+1]   // top neighbour 
                                                    - img->buffers.edge[slice.width][yi]
                                                    );
    }
}

void set_left_boundary(img_str *img, slc_str slice, int yi, double val)
{
	img->buffers.old[0][yi]   = (int)(255.0*(1.0-val));
    img->buffers.new[0][yi]   = (int)(255.0*(1.0-val));	
}

void set_right_boundary(img_str *img, slc_str slice, int yi, double val)
{
	img->buffers.old[slice.actual.width+1][yi] = (int)(255.0*val);
    img->buffers.new[slice.actual.width+1][yi] = (int)(255.0*val);	
}

void set_top_boundary(img_str *img, int xi)
{
	img->buffers.old[xi][img->size.height+1] = img->buffers.old[xi][1];
	img->buffers.new[xi][img->size.height+1] = img->buffers.new[xi][1];	
}

void set_bottom_boundary(img_str *img, int xi)
{
	img->buffers.old[xi][0] = img->buffers.old[xi][img->size.height];
	img->buffers.new[xi][0] = img->buffers.new[xi][img->size.height];	
}

double boundaryval(int i, int m)
{
  double val;

  val = 2.0*((double)(i-1))/((double)(m-1));
  if (i >= m/2+1) val = 2.0-val;
  
  return val;
}

void setup_periodic_boundaries(img_str *img)
{
	int xi;

	for(xi=1; xi < img->size.width+1; xi++)
	{
		set_top_boundary(img,xi);
		set_bottom_boundary(img,xi);
	}
}

void setup_sawtooth_boundaries(img_str *img, slc_str slice)
{
    int yi;
    double val;
    /* Set fixed boundary conditions on the leftmost and rightmost sides only */
    for (yi=1; yi < slice.actual.height+1; yi++)
    {
        /* compute sawtooth value */
        val = boundaryval(yi, img->size.height);
	    
    	set_left_boundary(img,slice,yi,val);
    	set_right_boundary(img,slice,yi,val);
    }
}

void setup_parallel_sawtooth_boundaries(cart_str cart, img_str *img, slc_str slice)
{
    int stidx, yi;
    double val;
    int block_height = (int) floor((double)img->size.height / (double)cart.dims[1]);
    /* Set fixed boundary conditions on the leftmost and rightmost sides only */
    for (yi=1; yi < slice.actual.height+1; yi++)
    {
        // compute sawtooth value 
        stidx = cart.coords[1] * block_height + yi;
        val = boundaryval(stidx, img->size.height);
	    
	    if(cart.coords[0]==0)	// set the leftmost
	    {
        	set_left_boundary(img,slice,yi,val);
        }
        if(cart.coords[0] == cart.dims[0]-1)	// set the rightmost
        {	
        	set_right_boundary(img,slice,yi,val);
    	}
    }
}

double get_local_maximum(img_str img, dim_str slice)
{
    double local_max_delta = 0.0;
    double delta;
    int xi,yi;

    for(xi=1; xi<slice.width+1; xi++)
    {
        for(yi=1; yi<slice.height+1; yi++)
        {
            delta = fabs(img.buffers.new[xi][yi] - img.buffers.old[xi][yi]);
            
            if(delta > local_max_delta) local_max_delta = delta;
        }
    }

    return local_max_delta;
}

double get_local_sum(img_str img, dim_str slice)
{
    double local_sum = 0.0;
    int xi,yi;

    for(xi=1; xi<slice.width+1; xi++)
    {
        for(yi=1; yi<slice.height+1; yi++)
        {
            local_sum += img.buffers.new[xi][yi];
        }
    }

    return local_sum;
}