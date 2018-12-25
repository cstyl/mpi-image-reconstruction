# Image Reconstructions using message-passing programming
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
- `test_resources/`: Contains script file were the user can specify the input arguments for each test.
- `res/`: Raw data of the experiments performed.
- `data/`: Images that can be used when running the program.
- `converter/`: Contains a program that can be used by the user to generate its own edge images.

## Options
Before compile and execute the code it is useful to know that different versions of the code can be compiled by specifying the `DEFINE` variable at the top of the `MAKEFILE`.

Available options are:

- `-DOVERLAP`: if set indicates overlapping communication
- `-DTIME`: is defined when the main loop needs to be timed.

Comment out accordingly which ones don't want to be used create a clean directory and recompile the code as it will be explained below.

Both the serial and parallel code come with the option to specify input arguments to the program through the command line. The available options are:

- `-ver`: The version of the code to be executed. If set to `0` the serial version will be executed. If set to `1` the parallel version will be executed. Default is `0`.
- `-ef`: The name of the edge file. It is assumed that the file is located in `data/`. Default is `edgenew192x128.pgm`.
- `-if`: The name of the reconstructed image file produced. It is stored in `out/` by default.
- `-i`: Number of iterations the code will be executed for. Default is `1000`. Default is `serial_image192x128.pgm`.
- `-mf`: Frequency at which the maximum absolute change is evaluated at. Default is `200`.
- `-af`: Frequency at which the average pixel value in the image is evaluated at. Default is `200`.

Note that these arguments are not mandatory. No check is performed about their correctness at the moment.
A sensible value for `-mf` or `-af` is 200.

## Usage

### Building
Select the desired `DEFINE` options from the `MAKEFILE` to compile the code with.

To compile the serial and parallel code use:
```sh
$ make all
```

This will result in two executables:
- `bin/image_no_overlap`: No overlapping communications and computations are used.
- `bin/image_overlap`: Communications and computations are overlapped.

In both cases, the processing loop is timed.

### Cleaning
To clean the project run:
```sh
$ make clean
```

### Running
To execute the serial code:
```sh
$ mpirun -n 1 ./bin/image_no_overlap -ver 0 -ef <string> -if <string> -i <int> -mf <int> -af <int>
```

To execute the parallel code:
```sh
$ mpirun -n <int> ./bin/image_no_overlap -ver 1 -ef <string> -if <string> -i <int> -mf <int> -af <int>
```
Note that the arguments in "< >" only indicate the suported type of input. Please specify your inputs as explained above.

If `-ver` is set to `0` the serial version of the code will be executed. If set to `1` the parallel version will be executed. Default option is set to be the serial.

Additionally, `./bin/image_no_overlap` can be substituted with `./bin/image_overlap` if overlapping communications are desired.

### Comparing outputs
To compare the generated outputs use:

```sh
$ diff a b
```
where `a` is the file of the serial image and `b` the file of the parallel image. Note that the two should be compiled with the same `DEFINE` statements and executed using the same parameters.

### Running the converter to use your own images
To generate your own edge images, use:

```sh
$ make conv NAME=archer.jpg WIDTH=400 HEIGHT=266
```

where `NAME` is the name of the image to be converted, located in `converter/`, `WIDTH` is the width of the image and `HEIGHT` the height of the image. The generated image is located in `data/`. Alternatively, one can follow the instructions in `converter/` under the `REAMDE.md`.

### Running correctness test
To execute the correctness tests do:
```sh
$ make ctest EXEC=bin/image_overlap
```

In order to run the test, the user needs to specify which version of the code desires to test. By setting `EXEC=bin/image_overlap` the version of the parallel code with overlapping communications is tested. If one wishes to test for non-overlaping communications then set `EXEC=bin/image_no_overlap`

Input arguments to the test can be specified by modifying the `test_resources/correctness_resources.sh`. Output results and logs can be found in `res/correctness/`. Test checks correctness for both early stopping criterion and fixed number of iterations. The source directory of the edge images is `data/`.

### Running performance test using a single node 
This test is about measuring the performance of the parallel code when executed on a single node i.e ignoring network communication overheads. A set of number of processes from 1 to 36 are tested.

To execute the test do:
```sh
$ make perf EXEC=bin/image_overlap REPS=5 ITER=20000
```
where `EXEC` is the name of the executable, `REPS` is the number of times the executable is executed for each process number and `ITER` is the maximum allowed iterations if early stopping criterion is not met.

### Running performance test using multiple nodes
This test is about measuring the performance of the parallel code when executed on multiple nodes i.e taking into consideration the network communication overheads. In order to obtain comparable results the same number of processes will be tested (from 6 to 36 choosing only multiple of 6).

To execute the test do:
```sh
$ make mulnodes EXEC=bin/image_overlap REPS=5 ITER=20000
```
where `EXEC` is the name of the executable, `REPS` is the number of times the executable is executed for each process number and `ITER` is the maximum allowed iterations if early stopping criterion is not met. The test runs for 1,2 and 3 nodes.

