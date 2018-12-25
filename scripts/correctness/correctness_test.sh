#!/bin/bash
: '
Description:
	Executes the serial code for a predifined edges inputs
	Then executes the parallel code for the same image each time for a set of number of processes
	Compares each parallel image with the corresponding serial image
	Checks for correctness i.e if the two images are the same
	The test is performed for a fixed number of iterations and then for early stopping
	Log file and out file are generated as well as on-screen messages informing the user about
	the results.
'
: '
Inputs:
	- EDGES = a set of edge images
	- IMAGES_SER = a set of the serial reconstructed images
	- IMAGES_PAR = a set of the parallel reconstructed images
	- PROC = a set of the number of processes to run the parallel code
	- EXEC = the name of the executable to be executed
	- OUT_DIR = output directory where the reconstructed images are located
	- RES_DIR = directory to store the results
Inputs are parse from ./test_resources/correctness_resources.sh
'
: '
Outputs:
	- logfile.txt: Contains test results.
	- out.txt: Contains the outputs of each executable run.
Both are located in ${resdir}
'

source ./test_resources/correctness_resources.sh

# Parse the parameters from the correctness_resources
EDGES=(${CORR_EDGES// / })
IMAGES_SER=(${CORR_SER_IMAGES// / })
IMAGES_PAR=(${CORR_PAR_IMAGES// / })
PROC=(${CORR_PROC// / })
EXEC=$CORR_EXEC

outdir=$CORR_OUT_DIR
resdir=$CORR_RES_DIR

logfile=$CORR_LOG_FILE
outfile=$CORR_OUT_FILE

ctr_fixed=0
ctr_early=0

echo "Testing ${EXEC}..."
echo "Generated outputs from correctness test.." > $outfile
echo "Running correctness test..." > $logfile
echo "Testing correctness for no early stopping..." >> $logfile

echo "Correctness test for fixed iterations.."
echo "Generating serial images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	# run the serial code - no early stop
	mpirun -n 1 $EXEC -ver 0 -ef ${EDGES[$i]} -if ${IMAGES_SER[$i]}  -i 50 -mf 0 -af 0 >> $outfile		
done

echo "Generating parallel images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	printf "Comparing ${IMAGES_SER[$i]} with ${IMAGES_PAR[$i]} on: \n" >> $logfile
	for j in `seq 0 $((${#PROC[@]}-1))`
	do	
		echo "  Testing for process number: ${PROC[$j]}"
		# run the parallel code - no early stop
		mpirun -n ${PROC[10#$j]} $EXEC -ver 1 -ef ${EDGES[$i]} -if ${IMAGES_PAR[$i]}  -i 50 -mf 0 -af 0 >> $outfile	
		printf "${PROC[10#$j]} processes: " >> $logfile
		
		DIFF=$(diff ${outdir}/${IMAGES_SER[$i]} ${outdir}/${IMAGES_PAR[$i]}) 
		if [ "$DIFF" != "" ] 
		then
		    printf "Test Failed:Images Differ.\n" >> $logfile
			ctr_fixed=$((ctr_fixed + 1))
		else
			printf "Test Pass.\n" >> $logfile
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
	mpirun -n 1 $EXEC -ver 0 -ef ${EDGES[$i]} -if ${IMAGES_SER[$i]}  -i 10000 -mf 1 -af 0 >> $outfile		
done

echo "Generating parallel images..."
for i in `seq 0 $((${#EDGES[@]}-1))`
do
	echo "Image ${EDGES[$i]}"
	printf "Comparing ${IMAGES_SER[$i]} with ${IMAGES_PAR[$i]} on: \n" >> $logfile
	for j in `seq 0 $((${#PROC[@]}-1))`
	do	
		echo "  Testing for process number: ${PROC[$j]}"
		# run the parallel code - no early stop
		mpirun -n ${PROC[10#$j]} $EXEC -ver 1 -ef ${EDGES[$i]} -if ${IMAGES_PAR[$i]}  -i 10000 -mf 1 -af 0 >> $outfile	

		printf "${PROC[10#$j]} processes: " >> $logfile

		DIFF=$(diff ${outdir}/${IMAGES_SER[$i]} ${outdir}/${IMAGES_PAR[$i]}) 
		if [ "$DIFF" != "" ] 
		then
		    printf "Test Failed:Images Differ.\n" >> $logfile
			ctr_early=$((ctr_early + 1))
		else
			printf "Test Pass.\n" >> $logfile
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
