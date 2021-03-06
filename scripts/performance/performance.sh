#!/bin/bash
: '
Description:
	Script to run and submit performance tests on the back-end of CIRRUS 
	using the performance.pbs script located in ./res/pbs/.
	Parses the command line arguments and determines which performance test is to be performed.
	Two options of tests are available:
		- Single node performance test
		- Multiple nodes performance test
	Note that it assumes correct input arguments and does not check for their correctness.
'
: '
Inputs:
	Command line arguments below:
		- EXEC = the name of the executable to be executed
		- REPS = number of repetitions each experiment is performed
		- ITERS = number of maximum allowed iterations of the main loop of parallel code
		- TEST = name of the test to be performed i.e singlenode or mulnodes
		- NODES = number of nodes required by the test
		- TIME = requested time to commit the resources on CIRRUS
		- RES = reservation name if exists
'
: '
Outputs:
	- logfile.txt: Contains cirrus logs.
	- perf.csv: Contains the time measurements from each test.
Both are located in ${RES_DIR}
'

printf "Parsing command line arguments...\n"

POSITIONAL=()
while [[ $# -gt 0 ]]
do
	key="$1"

	case $key in
	    -e|--executable)
	    EXEC="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -r|--repetitions)
	    REPS="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -i|--iterations)
	    ITERS="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -T|--test)
	    TEST="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -n|--nodes)
	    NODES="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -t|--time)
	    T="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    -R|--reservation)
	    RES="$2"
	    shift # past argument
	    shift # past value
	    ;;
	    *)    # unknown option
	    echo "$1 is an invalid option and discarded"
	    shift # past argument
	    ;;
	esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

if [ "$EXEC" = "bin/image_overlap" ]
then
	CASE=overlap
	make bin/image_overlap DEFINE=-DOVERLAP DEFINE+=-DTIME
elif [ "$EXEC" = "bin/image_no_overlap" ]
then
	CASE=no_overlap
	make bin/image_no_overlap DEFINE=-DTIME
fi

printf "Parsing Completed.\nSetting up ${CASE}_${TEST}${NODES} test...\n"

# number of processes where the parallel code will be executed on
if [ "$TEST" = "perf" ]
then
	PROC="1 2 4 8 12 15 16 23 25 28 31 32 36"
elif [ "$TEST" = "mulnodes" ]
then
	PROC="6 12 18 24 30 36"
fi

RESERVATION=${RES}
ACCOUNT=d167-s1887443				# account to charge
NAME=${CASE}_${TEST}${NODES}		# name of the backend job
SELECT=select=${NODES}:ncpus=36		# resources reserved
TIME=walltime=${T}
PLACE=place=excl

# edge images located in data/
EDGES="edgenew192x128.pgm edgenew256x192.pgm edgenew512x384.pgm edgenew768x768.pgm"
# names of parallel images to be generated in out/
IMAGES="${NAME}_parallel_image192x128.pgm ${NAME}_parallel_image256x192.pgm ${NAME}_parallel_image512x384.pgm ${NAME}_parallel_image768x768.pgm"

OUT_DIR=out
RES_DIR=res/performance/${NAME}
PBS_DIR=scripts/pbs
mkdir -p ${RES_DIR}

TEMP_FILE=${OUT_DIR}/${NAME}_out.txt
TEMP_RES_FILE=${OUT_DIR}/${NAME}_temp_res.txt
PERF_FILE=${RES_DIR}/${NAME}_perf.csv
LOG_FILE=${RES_DIR}/${NAME}_log.txt

printf "Setup completed.\nSubmitting test on backend nodes...\n"

qsub $RESERVATION -v PROC="${PROC}",EDGES="${EDGES}",IMAGES="${IMAGES}",EXEC="${EXEC}",TEMP_FILE="$TEMP_FILE",TEMP_RES_FILE="$TEMP_RES_FILE",PERF_FILE="$PERF_FILE",LOG_FILE="$LOG_FILE",REPS="$REPS",ITERS="$ITERS" \
				  -N $NAME -A $ACCOUNT \
				  -l $SELECT,$TIME,$PLACE \
				  -e $OUT_DIR -o $OUT_DIR\
				  ${PBS_DIR}/performance.pbs

printf "Submission successful.\n"
