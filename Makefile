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

MPLIB = $(SRC)/mplib
IMGLIB = $(SRC)/imglib
UTIL = $(SRC)/util
SER = $(SRC)/serlib
MPI2D = $(SRC)/parlib
WRAPLIB = $(SRC)/wraplib

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
all: dir $(BIN)/image

.PHONY: dir
## dir: create directories
dir:
	mkdir -p $(BIN) $(OBJ) $(OUT)
	mkdir -p $(RES)/correctness $(RES)/performance

## ctest: perform correctness tests.
.PHONY: ctest
ctest:
	@chmod u+x scripts/correctness/correctness_test.sh
	./scripts/correctness/correctness_test.sh

## conv: runs the converter to generate the desired edge image
.PHONY: conv
conv:
	@chmod u+x converter/converter.sh
	./converter/converter.sh -n $(NAME) -w $(WIDTH) -h $(HEIGHT)

# compile all c files and create the output files
$(OBJ)/%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

# link the output files to create the executable
$(BIN)/image: $(UTIL_OBJ) $(IMG_OBJ) $(MP_OBJ) $(VER_OBJ) $(MAIN_OBJ)
	$(CC) $^ -o $@ $(LFLAGS)		

.PHONY: clean
	## clean: clean directory
clean:
	rm -rf $(OBJ)/* $(BIN)/* $(OUT)/*