#!/bin/bash
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

printf "Parsing Completed.\nSetting up ${CASE}_${TEST} test...\n"

RESERVATION=${RES}
ACCOUNT=d167-s1887443				# account to charge
NAME=${CASE}_${TEST}				# name of the backend job
SELECT=select=1:ncpus=36			# resources reserved
TIME=walltime=${T}
PLACE=place=excl

# edge images located in data/
EDGES="edgenew192x128.pgm edgenew256x192.pgm edgenew512x384.pgm edgenew768x768.pgm"
# names of parallel images to be generated in out/
IMAGES="${NAME}_parallel_image192x128.pgm ${NAME}_parallel_image256x192.pgm ${NAME}_parallel_image512x384.pgm ${NAME}_parallel_image768x768.pgm"
PROC="2 4 8 12 19"

OUT_DIR=out
RES_DIR=res/overheads/${NAME}
PBS_DIR=scripts/pbs
mkdir -p ${RES_DIR}

TEMP_FILE=${OUT_DIR}/${NAME}_out.txt
TEMP_RES_FILE=${OUT_DIR}/${NAME}_temp_res.txt
RES_FILE=${RES_DIR}/${NAME}_result.csv
LOG_FILE=${RES_DIR}/${NAME}_log.txt

printf "Setup completed.\nSubmitting test on backend nodes...\n"

qsub $RESERVATION -v PROC="${PROC}",EDGES="${EDGES}",IMAGES="${IMAGES}",EXEC="${EXEC}",TEST="${TEST}",TEMP_FILE="$TEMP_FILE",TEMP_RES_FILE="$TEMP_RES_FILE",RES_FILE="$RES_FILE",LOG_FILE="$LOG_FILE",REPS="$REPS",ITERS="$ITERS" \
				  -N $NAME -A $ACCOUNT \
				  -l $SELECT,$TIME,$PLACE \
				  -e $OUT_DIR -o $OUT_DIR\
				  ${PBS_DIR}/overheads.pbs

printf "Submission successful.\n"
