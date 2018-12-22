#ifndef __ARGS_H__
#define __ARGS_H__

#include "structs.h"

int read_filenames(int an, char **av, img_str *image, int version);
int read_parameters(int an, char **av, params_str *params);
void print_params_filenames(img_str img, params_str params);

#endif	//__ARGS_H__