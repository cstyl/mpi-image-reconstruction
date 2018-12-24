#!/bin/bash
# converts an image into the edge image
# takes as inputs:
#   -n: the name of the image to be converted
#   -w: the width of the image
#   -h: the height of the image
# returns:
#   the edge image located in data/

cd converter/

#get user-defined modifications to parameters
while getopts ":n:w:h:" opt; do
  case $opt in
    n) NAME=$OPTARG
      ;;
    w)
      WIDTH=$OPTARG
      ;;
    h)
      HEIGHT=$OPTARG
      ;;
     
  esac
done

# Create the grayscale pgm image
convert $NAME -colorspace gray -compress none -depth 8 image${WIDTH}x${HEIGHT}.pgm
# compile the edge generator and create an executable
gfortran -o pgm2edge pgm2edge.f90
# run executable and generate the edge file
./pgm2edge image${WIDTH}x${HEIGHT}.pgm edgenew${WIDTH}x${HEIGHT}.pgm
# copy the edge file in data/
cp edgenew${WIDTH}x${HEIGHT}.pgm ../data
# clean up generated files in converter/
rm edgenew${WIDTH}x${HEIGHT}.pgm image${WIDTH}x${HEIGHT}.pgm ipgmio.mod pgm2edge