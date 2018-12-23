# mpi-image-reconstruction
Two-dimensional lattice-based calculation that employs a two-dimensional domain decomposition and uses non-blocking communications for image reconstruction

## What is included
- `include/`: Contains the header file called `structs.h`. This contains all the derived data structures used in the development of the code.
- `src/imglib/`: Contains all the functions used to perform the image reconstruction.
- `src/mplib/`: Contains all the functions used to parallelize the code using message-passing programming.
- `src/parlib/`: Contains all the wrap functions used to generate the parallel version of the project.
- `src/serlib/`: Contains all the wrap functions used to generate the serial version of the the project.
- `src/util/`: Contains all the helper functions used in the project.
	- `args.h`: Functions that parse the command line input in the project and obtain the desired parameters and file names.
	- `arralloc.h`: Provided file that contains a function to declare an N-dimensional array avoiding the problems occuring by `malloc`.
	- `mem.h`: Contains functions that allocate and deallocate desired buffers for each implementation. Also, a function that swaps pointers to avoid copying data in each buffer.
	- `pgmio.h`: Contains functions that read the data from a `.pgm` file and stores them in buffers. Also, functions that write back the data in a `.pgm` file from the buffers.
- `scripts/correctness/`: Contains the scripts used to check the parallel code for correctness.
- `scripts/performance/`: Contains the scripts used to measure the performance of the parallel code.
- `res/`: Raw data of the experiments performed.
- `data/`: Images that can be used when running the program.
- `converter/`: Contains a program that can be used by the user to generate its own edge images.

## Usage
Before compile and execute the code it is useful to know that different versions of the code can be compiled by specifying the `DEFINE` variable at the top of the `MAKEFILE`.
`-DOVERLAP`: if set indicates overlapping communication
`-DTIME`: is defined when the main loop needs to be timed.

Comment out accordingly which ones don't want to be used create a clean directory and recompile the code as it will be explained below.

Both the serial and parallel code come with the option to specify input arguments to the program through the command line. The available options are:
`-ver`: The version of the code to be executed. If set to `0` the serial version will be executed. If set to `1` the parallel version will be executed. Default is `0`.
`-ef`: The name of the edge file. It is assumed that the file is located in `data/`. Default is `edgenew192x128.pgm`.
`-if`: The name of the reconstructed image file produced. It is stored in `out/` by default.
`-i`: Number of iterations the code will be executed for. Default is `1000`. Default is `serial_image192x128.pgm`.
`-mf`: Frequency at which the maximum absolute change is evaluated at. Default is `200`.
`-af`: Frequency at which the average pixel value in the image is evaluated at. Default is `200`.

Note that these arguments are not mandatory. No check is performed about their correctness at the moment.
A sensible value for -mf or -af is 200.

### Usage

#### Building
Select the desired `DEFINE` options from the `MAKEFILE` to compile the code with.

To compile the serial and parallel code use:
```sh
$ make all
```

#### Cleaning
To clean the project run:
```sh
$ make clean
```

#### Running
To execute the serial code:
```sh
$ mpirun -n 1 ./bin/image -ver 0 -ef <string> -if <string> -i <int> -mf <int> -af <int>
```

To execute the parallel code:
```sh
$ mpirun -n <int> ./bin/image -ver 1 -ef <string> -if <string> -i <int> -mf <int> -af <int>
```
Note that the arguments in "< >" only indicate the suported type of input. Please specify your inputs as explained above.

If `-ver` is set to `0` the serial version of the code will be executed. If set to `1` the parallel version will be executed. Default option is set to be the serial.

#### Comparing outputs
To compare the generated outputs use:

```sh
$ diff a b
```
where a is the file of the serial image and b the file of the parallel image. Note that the two should be compiled with the same `DEFINE` statements and executed using the same parameters.

#### Running correctness test
To execute the correctness tests do:
```sh
$ make ctest
```

and

```sh
$ make ctests_early
```

Note that the first requires compilation with the -DEARLY_STOP commented out while the second requires it to be defined.
