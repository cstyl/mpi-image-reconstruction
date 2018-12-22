#ifndef __PGMIO_H__
#define __PGMIO_H__

#include "structs.h"

void pgmsize (char *filename, int *nx, int *ny);
void pgmread (char *filename, void *vx, int nx, int ny);
void pgmwrite(char *filename, void *vx, int nx, int ny);

void pgmread_generalised_cascaded(char *filename, void *vx, cart_str cart, dim_str img, slc_str slice);
void pgmwrite_generalised_cascaded(char *filename, void *vx, cart_str cart, dim_str img, slc_str slice);

#endif	//__PGMIO_H__