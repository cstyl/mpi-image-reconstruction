#!/bin/bash
# Executes the serial code for a predifined edges inputs
# Then executes the parallel code for the same image each time for a set of number of processes
# Compares each parallel image with the corresponding serial image
# Checks for correctness i.e if the two images are the same
# The test is performed for a fixed number of iterations and then for early stopping
# Log file and out file are generated as well as on-screen messages informing the user about
# the results.
EDGES=("edgenew192x128.pgm" "edgenew256x192.pgm" "edgenew512x384.pgm" "edgenew768x768.pgm")

IMAGES_SER=("serial_image192x128.pgm" "serial_image256x192.pgm" 
			"serial_image512x384.pgm" "serial_image768x768.pgm")

IMAGES_PAR=("parallel_image192x128.pgm" "parallel_image256x192.pgm" 
			"parallel_image512x384.pgm" "parallel_image768x768.pgm")

PROC=("1" "2" "4" "5" "8" 
	  "12" "15" "16" "19" 
	  "23" "25" "27" "29"
	  "31" "32" "36" "38"
	  "40" "43" "45" "47"
	  "53" "55" "57" "59" 
	  "60" "61" "62" "67")

outdir="out"

resdir="res/correctness"
logfile="${resdir}/logfile.txt"
outfile="${resdir}/out.txt"

ctr_fixed=0
ctr_early=0

echo "Generated outputs from correctness test.." > $outfile
echo "Running correctness test..." > $logfile
echo "Testing correctness for no early stopping..." >> $logfile

echo "Correctness test for fixed iterations.."
echo "Generating serial images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	# run the serial code - no early stop
	mpirun -n 1 bin/image -ver 0 -ef ${EDGES[$i]} -if ${IMAGES_SER[$i]}  -i 50 -mf 0 -af 0 >> $outfile		
done

echo "Generating parallel images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	for j in `seq 0 $((${#PROC[@]}-1))`
	do	
		echo "  Testing for process number: ${PROC[$j]}"
		# run the parallel code - no early stop
		mpirun -n ${PROC[10#$j]} bin/image -ver 1 -ef ${EDGES[$i]} -if ${IMAGES_PAR[$i]}  -i 50 -mf 0 -af 0 >> $outfile	
		
		DIFF=$(diff ${outdir}/${IMAGES_SER[$i]} ${outdir}/${IMAGES_PAR[$i]}) 
		if [ "$DIFF" != "" ] 
		then
		    echo "Images Differ. Tested on -n ${PROC[10#$j]} bin/image -ver 1 -ef ${EDGES[$i]} -if ${IMAGES_PAR[$i]}  -i 50 -mf 0 -af 0" >> $logfile
			ctr_fixed=$((ctr_fixed + 1))
		fi	
	done
done

echo "Testing correctness for early stopping..." >> $logfile

echo "Correctness test for early stopping.."
echo "Generating serial images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	# run the serial code - no early stop
	mpirun -n 1 bin/image -ver 0 -ef ${EDGES[$i]} -if ${IMAGES_SER[$i]}  -i 10000 -mf 1 -af 0 >> $outfile		
done

echo "Generating parallel images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	for j in `seq 0 $((${#PROC[@]}-1))`
	do	
		echo "  Testing for process number: ${PROC[$j]}"
		# run the parallel code - no early stop
		mpirun -n ${PROC[10#$j]} bin/image -ver 1 -ef ${EDGES[$i]} -if ${IMAGES_PAR[$i]}  -i 10000 -mf 1 -af 0 >> $outfile	
		
		DIFF=$(diff ${outdir}/${IMAGES_SER[$i]} ${outdir}/${IMAGES_PAR[$i]}) 
		if [ "$DIFF" != "" ] 
		then
		    echo "Images Differ. Tested on -n ${PROC[10#$j]} bin/image -ver 1 -ef ${EDGES[$i]} -if ${IMAGES_PAR[$i]}  -i 10000 -mf 1 -af 0" >> $logfile
			ctr_early=$((ctr_early + 1))
		fi	
	done
done

if [ "$ctr_fixed" -eq 0 ]
then
	echo "All test passed.. Code works for fixed number of iterations!"
else
	echo "Some tests failed. Check logfile in res/correctness/"
fi

if [ "$ctr_early" -eq 0 ]
then
	echo "All test passed.. Code works when early stopping is used!"
else
	echo "Some tests failed. Check logfile in res/correctness/"
fi

echo "Correctness test completed..." >> $logfile
echo "Correctness test completed..."
