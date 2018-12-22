#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

int read_filenames(int an, char **av, img_str *image, int version)
{
	int ai = 1;

	// default file names
	strcpy(image->efilename, "data/edgenew192x128.pgm");
	if(version==serial)
	{
		strcpy(image->ifilename,"out/serial_");
	}else if(version == par2D)
	{
		strcpy(image->ifilename,"out/parallel_");
	}
	strcat(image->ifilename, "image192x128.pgm");

	while(ai<an)
	{
		if(!strcmp(av[ai],"-ef"))
		{
			strcat(strcpy(image->efilename, "data/"), av[ai+1]);
		}else if(!strcmp(av[ai],"-if"))
		{
			strcat(strcpy(image->ifilename, "out/"), av[ai+1]);
		}
		ai+=1;
	}

	return 0;

}

int read_parameters(int an, char **av, params_str *params)
{
	int ai = 1;

	// default parameter values
	params->iterations = 1000;
	params->delta_freq = 200;
	params->avg_freq = 200;

	while(ai<an)
	{
		if(!strcmp(av[ai],"-i"))
		{
			params->iterations = atoi(av[ai+1]);
		}else if(!strcmp(av[ai],"-mf"))
		{
			params->delta_freq = atoi(av[ai+1]);
		}else if(!strcmp(av[ai],"-af"))
		{
			params->avg_freq = atoi(av[ai+1]);
		}
		ai+=1;
	}

	return 0;
}

void print_params_filenames(img_str img, params_str params)
{
	printf("Selected Parameters:\n");
	printf("\tInput File: <%s>\n\tOutput File: <%s>\n", img.efilename, img.ifilename);
	printf("\tNumber of Iterations: %d\n\tDelftaFreq: %d\n\tAvgFreq: %d\n",
						params.iterations, params.delta_freq, params.avg_freq);
}