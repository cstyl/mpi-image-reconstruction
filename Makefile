CC=	mpicc

# -DOVERLAP if set indicates overlapping communication
# -DTIME is defined when the main loop needs to be timed.
# Comment out accordingly which ones don't want to be used
# and recompile the code.
DEFINE+= -DOVERLAP
DEFINE+= -DTIME

CCFLAGS= -O3 -Wall -std=c99
CCFLAGS+= $(DEFINE)

LFLAGS=	-lm

INC = include
SRC = src
OBJ = obj
BIN = bin
OUT = out
DATA = data
RES = res
TEST_RES = test_resources
SCRIPTS = scripts

MPLIB = $(SRC)/mplib
IMGLIB = $(SRC)/imglib
UTIL = $(SRC)/util
SER = $(SRC)/serlib
MPI2D = $(SRC)/parlib
WRAPLIB = $(SRC)/wraplib

CORR = $(SCRIPTS)/correctness
PERF = $(SCRIPTS)/performance
OVERH = $(SCRIPTS)/overheads
PLOTS = $(SCRIPTS)/plots
PBS = $(SCRIPTS)/pbs

VPATH = $(SRC) $(INC) $(MPLIB) $(IMGLIB) $(UTIL) $(SER) $(MPI2D) $(WRAPLIB)
INCLUDES = -I$(INC) -I$(MPLIB) -I$(IMGLIB) -I$(UTIL) \
		   -I$(MPI2D) -I$(SER) -I$(WRAPLIB)

UTIL_OBJ =  $(OBJ)/mem.o  $(OBJ)/args.o $(OBJ)/arralloc.o $(OBJ)/pgmio.o
IMG_OBJ = $(OBJ)/imglib.o
MP_OBJ = $(OBJ)/mplib.o

VER_OBJ = $(OBJ)/serlib.o $(OBJ)/parlib.o $(OBJ)/wraplib.o

MAIN_OBJ = $(OBJ)/main.o

.PHONY: all
## all: compile and create the executables
all: dir 
	make $(BIN)/image_overlap DEFINE=-DOVERLAP DEFINE+=-DTIME -B
	make $(BIN)/image_no_overlap DEFINE=-DTIME -B

.PHONY: dir
## dir: create directories
dir:
	mkdir -p $(BIN) $(OBJ) $(OUT)
	mkdir -p $(RES)/correctness $(RES)/performance

## ctest: perform correctness tests.
.PHONY: ctest
ctest:
	@chmod u+x $(CORR)/correctness_test.sh
	@chmod u+x $(TEST_RES)/correctness_resources.sh

	@./$(CORR)/correctness_test.sh -e $(EXEC)

## perf: perform basic performance test over a number of processes.
.PHONY: perf
perf:
	@chmod u+x $(PERF)/performance.sh
	@chmod u+x $(PBS)/performance.pbs
	@./$(PERF)/performance.sh -e $(EXEC) -r $(REPS) -i $(ITER) \
							  -T perf -n 1 -t 00:20:00 \
							  -R $(RESERV)

## mulnodes: perform multiple nodes performance tests.
.PHONY: mulnodes
mulnodes:
	@chmod u+x $(PERF)/performance.sh
	@chmod u+x $(PBS)/performance.pbs
	@./$(PERF)/performance.sh -e $(EXEC) -r $(REPS) -i $(ITER) \
							  -T mulnodes -n 1 \
							  -t 00:20:00 -R $(RESERV)
	@./$(PERF)/performance.sh -e $(EXEC) -r $(REPS) -i $(ITER) \
							  -T mulnodes -n 2 \
							  -t 00:20:00 -R $(RESERV)
	@./$(PERF)/performance.sh -e $(EXEC) -r $(REPS) -i $(ITER) \
							  -T mulnodes -n 3 \
							  -t 00:20:00 -R $(RESERV)

## plotperf: plot individual performance results
.PHONY: plotperf
plotperf:
	@python $(PLOTS)/plots_$(TEST).py -v $(VER)

## plotperf_all: plot all performance results
.PHONY: plotperf_all
plotperf_all:
	@make plotperf TEST=perf VER=overlap
	@make plotperf TEST=perf VER=no_overlap
	@make plotperf TEST=mulnodes VER=overlap
	@make plotperf TEST=mulnodes VER=no_overlap

## overheads: perform basic overhead test over a number of processes.
.PHONY: overheads
overheads:
	@chmod u+x $(OVERH)/overheads.sh
	@chmod u+x $(PBS)/overheads.pbs
	@./$(OVERH)/overheads.sh -e $(EXEC) -r $(REPS) -i $(ITER) \
							  -T $(TEST) -t 00:20:00 -R $(RESERV)

## plotoverh: plot individual overhead results
.PHONY: plotoverh
plotoverh:
	@python $(PLOTS)/plots_overheads.py -v $(VER)

## plotoverh_all: plot all overhead results
.PHONY: plotoverh_all
plotoverh_all:
	@make plotoverh VER=overlap
	@make plotoverh VER=no_overlap

## conv: runs the converter to generate the desired edge image
.PHONY: conv
conv:
	@chmod u+x converter/converter.sh
	@./converter/converter.sh -n $(NAME) -w $(WIDTH) -h $(HEIGHT)

# compile all c files and create the output files
$(OBJ)/%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

# link the output files to create the executable
$(BIN)/image_overlap: $(UTIL_OBJ) $(IMG_OBJ) $(MP_OBJ) $(VER_OBJ) $(MAIN_OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

$(BIN)/image_no_overlap: $(UTIL_OBJ) $(IMG_OBJ) $(MP_OBJ) $(VER_OBJ) $(MAIN_OBJ)
	$(CC) $^ -o $@ $(LFLAGS)		

.PHONY: clean
## clean: clean directory
clean:
	rm -rf $(OBJ)/* $(BIN)/* $(OUT)/*

.PHONY: help
# help: prints each repice's purpose
help: Makefile
	@sed -n 's/^##//p' $<