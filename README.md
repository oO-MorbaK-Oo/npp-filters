# Image Filtering using NVIDIA NPP with CUDA

## Overview

This project demonstrates the use of NVIDIA Performance Primitives (NPP) library with CUDA to perform image filtering. The goal is to utilize GPU acceleration to efficiently filter a given image by a specified filter, leveraging the computational power of modern GPUs.

## Prerequisites

Download and install the [CUDA Toolkit 12.4](https://developer.nvidia.com/cuda-downloads) for your corresponding platform.

## Build and Run

### Windows
The Windows program is built using the Visual Studio 2022 IDE.

### Linux
The Linux program is built using makefiles. To use the makefiles, just run make:
```
$ make
```

## Running the Program
After building the project, you can run the program using the following command:

```bash
Copy code
make run
```

This command will execute the compiled binary, filtering the input image (Lena.png) by a 5x5 box filter with replicate border, and save the result as Lena_box_replicate.png in the data/ directory.

If you wish to run the binary directly with custom input/output files, you can use:

```bash
- Copy code
./bin/npp-filter --input data/Lena.png --filter box --border replicate --output data/Lena_rotated.png
```

- Cleaning Up
To clean up the compiled binaries and other generated files, run:


```bash
- Copy code
make clean
```

This will remove all files in the bin/ directory.
