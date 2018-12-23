# Converter Usage

You can create your own edge files for reconstruction with the program `pgm2edge`, provided you have an initial image in PGM format. You can create PGM images from, say, JPG images using `convert`. Note that this produces edge files using the new boundary conditions appropriate for the MPP coursework. Here, we demonstrate how a user can generate its own edge files from a `.jpg` image. An example image `archer.jpg` is used.

## Convert the `.jpg` image in grayscale `.pgm` image

```sh
convert archer.jpg -colorspace gray -compress none -depth 8 image400x266.pgm 
```

Note that in `image400x266.pgm` the `400x266` is the width and height of the image.

## Compile and generate an executable of the converter

```sh
gfortran -o pgm2edge pgm2edge.f90
```
Usage: pgm2edge <input image file> <output edge file>

## Run the converter

```sh
./pgm2edge image400x266.pgm edgenew400x266.pgm
 pgm2edge: infile = image400x266.pgm                outfile = edgenew400x266.pgm                 
 nx, ny =          400         266
 Reading          400  x          266  picture from file: image400x266.pgm                
 pgmread: image min, max =            0         255
 Writing          400  x          266  picture into file: edgenew400x266.pgm                 
 pgm2edge: finished
```

The generated edge file is called `edgenew400x266.pgm`

## Move the new edge file in the appropriate directory
```sh
cp edgenew400x266.pgm ../data/
```