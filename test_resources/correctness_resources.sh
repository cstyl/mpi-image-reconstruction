#!/bin/bash
set -o allexport

printf "Initialising resources for correctness test..."

#get the name of the executable to be tested
while getopts ":e:" opt; do
  case $opt in
    e)
      EXEC_ARG=$OPTARG
      ;;
  esac
done

if [ "$EXEC_ARG" = "bin/image_overlap" ]
then
	CASE=overlap
elif [ "$EXEC_ARG" = "bin/image_no_overlap" ]
then
	CASE=no_overlap
fi
# name of the executable to be tested
CORR_EXEC=${EXEC_ARG}
# edge images located in data/
CORR_EDGES="edgenew192x128.pgm edgenew256x192.pgm 
	        edgenew512x384.pgm edgenew768x768.pgm"
# names of parallel images to be generated in out/
CORR_PAR_IMAGES="${CASE}_parallel_image192x128.pgm ${CASE}_parallel_image256x192.pgm 
	             ${CASE}_parallel_image512x384.pgm ${CASE}_parallel_image768x768.pgm"
# names of serial images to be generated in out/
CORR_SER_IMAGES="${CASE}_serial_image192x128.pgm ${CASE}_serial_image256x192.pgm 
				 ${CASE}_serial_image512x384.pgm ${CASE}_serial_image768x768.pgm"
# number of processes where the parallel code will be executed on
CORR_PROC="1 2 4 5 8
	       12 15 16 19 
	  	   23 25 27 29
	  	   31 32 36 38
	  	   40 43 45 47
	  	   53 55 57 59 
	  	   60 61 62 67"

CORR_OUT_DIR=out
CORR_RES_DIR=res/correctness

CORR_LOG_FILE=${CORR_RES_DIR}/${CASE}_logfile.txt
CORR_OUT_FILE=${CORR_RES_DIR}/${CASE}_out.txt

printf "\tDone.\n"
