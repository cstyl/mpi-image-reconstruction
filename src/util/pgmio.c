/*
 * This file contains C routines for the MPI Casestudy.
 *
 * "pgmread" reads in a PGM picture and can be called as follows:
 *
 *    double buf[M][N];
 *    pgmread("edge.pgm", buf, M, N);
 *
 * "pgmwrite" writes an array as a PGM picture and can be called as follows:
 *
 *    double buf[M][N];
 *    pgmwrite("picture.pgm", buf, M, N);
 *
 * "pgmsize" returns the size of a PGM picture and can be called as follows:
 *
 *    int nx, ny;
 *    pgmsize("edge.pgm", &nx, &ny);
 *
 *  To access these routines, add the following to your program:
 *
 *    #include "pgmio.h"
 *
 *  Note: you MUST link with the maths library -lm to access fabs etc.
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "pgmio.h"
#define MAXLINE 128

/*
 *  Routine to get the size of a PGM data file
 *
 *  Note that this assumes a single line comment and no other white space.
 */

void pgmsize(char *filename, int *nx, int *ny)
{ 
  FILE *fp;

  char *cret;
  int iret;

  char dummy[MAXLINE];
  int n = MAXLINE;

  if (NULL == (fp = fopen(filename,"r")))
  {
    fprintf(stderr, "pgmsize: cannot open <%s>\n", filename);
    exit(-1);
  }

  if((cret = fgets(dummy, n, fp)) < 0);
      cret=" ";
  if((cret = fgets(dummy, n, fp)) < 0);
      cret=" ";

  if((iret = fscanf(fp,"%d %d", nx, ny)) < 0)
    iret = 0;

      
  fclose(fp);
}


/*
 *  Routine to read a PGM data file into a 2D floating point array
 *  x[nx][ny]. Because of the way C handles (or fails to handle!)
 *  multi-dimensional arrays we have to cast the pointer to void.
 *
 *  Note that this assumes a single line comment and no other white space.
 */

void pgmread(char *filename, void *vx, int nx, int ny)
{ 
  FILE *fp;

  int nxt, nyt, i, j, t;
  char dummy[MAXLINE];
  int n = MAXLINE;

  char *cret;
  int iret;

  double *x = (double *) vx;

  if (NULL == (fp = fopen(filename,"r")))
  {
    fprintf(stderr, "pgmread: cannot open <%s>\n", filename);
    exit(-1);
  }

  if((cret = fgets(dummy, n, fp)) < 0);
      cret=" ";
  if((cret = fgets(dummy, n, fp)) < 0);
      cret=" ";

  if((iret = fscanf(fp,"%d %d",&nxt,&nyt)) < 0)
    iret=0;
  
  if (nx != nxt || ny != nyt)
  {
    fprintf(stderr,
            "pgmread: size mismatch, (nx,ny) = (%d,%d) expected (%d,%d)\n",
            nxt, nyt, nx, ny);
    exit(-1);
  }

  if((iret = fscanf(fp,"%d",&i)) < 0)
    iret=0;
  /*
   *  Must cope with the fact that the storage order of the data file
   *  is not the same as the storage of a C array, hence the pointer
   *  arithmetic to access x[i][j].
   */
  // increase row
  for (j=0; j<ny; j++)
  {
    // read row
    for (i=0; i<nx; i++)
    {
      iret = fscanf(fp,"%d", &t);
      // (ny-j-1)+ny*i access the right memory
      x[(ny-j-1)+ny*i] = t;
    }
  }

  fclose(fp);
}

/*
 *  Routine to write a PGM image file from a 2D floating point array
 *  x[nx][ny]. Because of the way C handles (or fails to handle!)
 *  multi-dimensional arrays we have to cast the pointer to void.
 */

void pgmwrite(char *filename, void *vx, int nx, int ny)
{
  FILE *fp;

  int i, j, k, grey;

  double xmin, xmax, tmp, fval;
  double thresh = 255.0;

  double *x = (double *) vx;

  if (NULL == (fp = fopen(filename,"w")))
  {
    fprintf(stderr, "pgmwrite: cannot create <%s>\n", filename);
    exit(-1);
  }

  printf("Writing %d x %d picture into file: %s\n", nx, ny, filename);

  /*
   *  Find the max and min absolute values of the array
   */

  xmin = fabs(x[0]);
  xmax = fabs(x[0]);

  for (i=0; i < nx*ny; i++)
  {
    if (fabs(x[i]) < xmin) xmin = fabs(x[i]);
    if (fabs(x[i]) > xmax) xmax = fabs(x[i]);
  }

  if (xmin == xmax) xmin = xmax-1.0;
  
  fprintf(fp, "P2\n");
  fprintf(fp, "# Written by pgmio::pgmwrite\n");
  fprintf(fp, "%d %d\n", nx, ny);
  fprintf(fp, "%d\n", (int) thresh);

  k = 0;

  for (j=ny-1; j >=0 ; j--)
  {
    for (i=0; i < nx; i++)
    {
      /*
       *  Access the value of x[i][j]
       */

      tmp = x[j+ny*i];

      /*
       *  Scale the value appropriately so it lies between 0 and thresh
       */

      fval = thresh*((fabs(tmp)-xmin)/(xmax-xmin))+0.5;
      grey = (int) fval;

      fprintf(fp, "%3d ", grey);

      if (0 == (k+1)%18) fprintf(fp, "\n");

      k++;
    }
  }

  if (0 != k%18) fprintf(fp, "\n");
  fclose(fp);
}

// cascades slices into a long memory layout
void pgmread_generalised_cascaded(char *filename, void *vx, cart_str cart, dim_str img, slc_str slice)
{ 
  FILE *fp;  
  /* Cartesian indexes for each memory block */
  int block_x,block_y;
  /* Cartesian indexes of each pixel inside each memory block */
  int block_widx, block_hidx;
  /* Current memory block */
  int block_idx;

  int nxt, nyt, i, t, idx;
  char dummy[MAXLINE];
  int n = MAXLINE;

  char *cret;
  int iret;

  double *x = (double *) vx;

  if (NULL == (fp = fopen(filename,"r")))
  {
    fprintf(stderr, "pgmread: cannot open <%s>\n", filename);
    exit(-1);
  }

  if((cret = fgets(dummy, n, fp)) < 0);
      cret=" ";
  if((cret = fgets(dummy, n, fp)) < 0);
      cret=" ";

  if((iret = fscanf(fp,"%d %d",&nxt,&nyt)) < 0)
    iret=0;

  if (img.width != nxt || img.height != nyt)
  {
    fprintf(stderr,
            "pgmread: size mismatch, (nx,ny) = (%d,%d) expected (%d,%d)\n",
            nxt, nyt, img.width, img.height);
    exit(-1);
  }

  if((iret = fscanf(fp,"%d",&i)) < 0)
    iret=0;

  /*
   *  Must cope with the fact that the storage order of the data file
   *  is not the same as the storage of a C array, hence the pointer
   *  arithmetic to access x[i][j].
   */
  for(block_y=cart.dims[1]-1; block_y>=0; block_y--)
  { 
    for(block_hidx=slice.padded.height-1; block_hidx>=0; block_hidx--)
    { 
      for(block_x=0; block_x<cart.dims[0]; block_x++)
      {
        for(block_widx=0; block_widx<slice.padded.width; block_widx++)
        {
            block_idx = block_x * cart.dims[1] + block_y;
            idx = slice.padded.height * slice.padded.width * block_idx + 
                  slice.padded.height * block_widx + block_hidx;
            // zero-pad additional memory space within each block if not on the boundaries
            if((!(block_y==cart.dims[1]-1) && (block_hidx >= slice.padded.height - slice.rem.height))||
               (!(block_x==cart.dims[0]-1) && (block_widx >= slice.padded.width  - slice.rem.width )))
            {
              t=0;
            }
            else
            {
              // read a pixel from the file
              if((iret = fscanf(fp,"%d", &t)) < 0)
                iret=0;
            }

            x[idx] = t;
        }
      }
    } 
  }

  fclose(fp);
}

void pgmwrite_generalised_cascaded(char *filename, void *vx, cart_str cart, dim_str img, slc_str slice)
{
  FILE *fp;
  /* Cartesian indexes for each memory block */
  int block_x,block_y;
  /* Cartesian indexes of each pixel inside each memory block */
  int block_widx, block_hidx;
  /* Current memory block */
  int block_idx;

  int idx, k, grey;

  double xmin, xmax, tmp, fval;
  double thresh = 255.0;

  double *x = (double *) vx;

  if (NULL == (fp = fopen(filename,"w")))
  {
    fprintf(stderr, "pgmwrite: cannot create <%s>\n", filename);
    exit(-1);
  }

  printf("Writing %d x %d picture into file: %s\n", img.width, img.height, filename);

  /*
   *  Find the max and min absolute values of the array
   */

  xmin = fabs(x[0]);
  xmax = fabs(x[0]);

  for(block_y=cart.dims[1]-1; block_y>=0; block_y--)
  { 
    for(block_hidx=slice.padded.height-1; block_hidx>=0; block_hidx--)
    { 
      for(block_x=0; block_x<cart.dims[0]; block_x++)
      {
        for(block_widx=0; block_widx<slice.padded.width; block_widx++)
        {
            block_idx = block_x * cart.dims[1] + block_y;
            idx = slice.padded.height * slice.padded.width * block_idx + 
                  slice.padded.height * block_widx + block_hidx;

            if(!(!(block_y==cart.dims[1]-1) && (block_hidx>=slice.padded.height - slice.rem.height))&&
               !(!(block_x==cart.dims[0]-1) && (block_widx>=slice.padded.width  - slice.rem.width)))
            {
              if (fabs(x[idx]) < xmin) xmin = fabs(x[idx]);
              if (fabs(x[idx]) > xmax) xmax = fabs(x[idx]);
            }    
        }
      }
    }
  }
  if (xmin == xmax) xmin = xmax-1.0;

  fprintf(fp, "P2\n");
  fprintf(fp, "# Written by pgmio::pgmwrite\n");
  fprintf(fp, "%d %d\n", img.width, img.height);
  fprintf(fp, "%d\n", (int) thresh);

  k = 0;

  for(block_y=cart.dims[1]-1; block_y>=0; block_y--)
  { 
    for(block_hidx=slice.padded.height-1; block_hidx>=0; block_hidx--)
    { 
      for(block_x=0; block_x<cart.dims[0]; block_x++)
      {
        for(block_widx=0; block_widx<slice.padded.width; block_widx++)
        {
            block_idx = block_x * cart.dims[1] + block_y;
            idx = slice.padded.height * slice.padded.width * block_idx + 
                  slice.padded.height * block_widx + block_hidx;

            if(!(!(block_y==cart.dims[1]-1) && (block_hidx>=slice.padded.height - slice.rem.height))&&
               !(!(block_x==cart.dims[0]-1) && (block_widx>=slice.padded.width  - slice.rem.width)))
            {
              /*
               *  Access the value of x[i][j]
               */
              tmp = x[idx];

              /*
               *  Scale the value appropriately so it lies between 0 and thresh
               */
              fval = thresh*((fabs(tmp)-xmin)/(xmax-xmin))+0.5;
              grey = (int) fval;

              fprintf(fp, "%3d ", grey);

              if (0 == (k+1)%18) fprintf(fp, "\n");

              k++; 
            }
        }
      }
    }
  }

  if (0 != k%18) fprintf(fp, "\n");
  fclose(fp);
}
