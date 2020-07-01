# simd-puzzles

Algorithmic problems optimized with SIMD

## Motivation

During some software projects I had have to optimize different types of algorithms to improve their speed with SIMD techniques. Basically I've made a fallback version of an algorithm without SIMD. After that I've tried to write a SIMD version of the algorithm and push this version as fast as I can. In most cases, I did this without being able to compare different SIMD implementations of an algorithm because there was no suitable test environment for different implementation approaches.

With this project I would like to collect some algorithmic SIMD puzzles and compare these on different architectures and compilers. Furthermore I hope some other guys [contributing](./CONTRIBUTING.md) algorithms puzzles and optimize these with SIMD. :smiley:

## Puzzles

| Algorithm | AVX2 | AVX-512 | NEON |
|-----------|------|---------|------|
| [Base64 Encode/Decode](./doc/base64/base64.md) | :heavy_check_mark: | :x: | :x: |
| [Scale Bytes](./doc/scale/scale.md) | :heavy_check_mark: | :x: | :x: |
| [Sum Bytes](./doc/sum/sum.md) | :heavy_check_mark: | :x: | :x: |

## Planned

* Further more puzzles
* AVX-512 and NEON implementations
* Compare multithreaded executions (e.g. `std::async`, OpenMP, ...)
* Better diagrams to compare algorithms on different architectures
* ...

## Directory Structure

The main project directory structure is figured in the following tree.

    .
    ├── bench
    │   └── <puzzle benchmark scripts>
    ├── cmake
    ├── doc
    │   └── <puzzle docs>
    ├── docker
    ├── extern
    ├── src
    │   ├── libbenchmark
    │   ├── libsimd
    │   │   ├── algorithms
    │   │   │   └── <puzzle SIMD sources>
    │   │   └── utils
    │   └── <puzzle sources>
    └── test
        └── <puzzle unit tests>

The main directories are:

* bench: Containing scripts to generate benchmarks for each puzzle.
* cmake: CMake stuff.
* doc: Subdirectories containing documentation of all puzzles with their performance results.
* docker: Dockerfiles to build project based on different Linux systems.
* extern: Used GIT submodules.
* src:
    * libbenchmark: Library containing code to manage the results of testruns, testsuites and to generate the CSV result files.
    * libsimd:
        * algorithms: (SIMD) sources of each puzzle.
        * utils: Some utils needfull to develop the SIMD puzzles.
    * Puzzle relevant source.
* test: Unit tests for all puzzles to ensure the correctness of all puzzle approach algorithms.

## Building

### Requirements

* C++ 17 compiler
* CMake 3.6 or later
* CPU with at least AVX2 instruction set
* Rscript

### Example

#### Build At Console

To build the library on Unix/Linux try:

    mkdir build
    cd build
    cmake ..
    make -j`nproc`

#### Build With Docker

At the [docker](docker) subdirectory several Dockerfiles are available to build the library based on different Linux distributions.

To build a Docker image based on Debian Buster type:

    docker build -f Dockerfile.debian-buster --force-rm -t simd-puzzles:0.1 .

inside the [docker](docker) subdirectory. When building a Docker image all benchmark scripts are processed.

A bash script inside the Docker container could be started with:

    docker run -it simd-puzzles:0.1 /bin/bash

## Extend Puzzles

The following steps are necessary to create a new puzzle:

1. Create a new folder for your puzzle at directory `src/libsimd/algorithms`.
2. Create a non-vectorized implementation of your puzzle. This is helpful to have a comparision of speedup against your (multiple) vectorization strategies.
3. Create a new folder inside your puzzle folder for your SIMD technique (e.g. `src/libsimd/algorithms/`*(puzzle)*`/avx2`).
4. Create a `.cpp` and `.hpp` file for your vectorization strategy.
5. Extend `CMakeLists.txt` at directory `src/libsimd/algorithms` with your `.cpp` and `.hpp` files.
6. Create a unit test for your puzzle at directory `test`.
7. Extend `CMakeLists.txt` at directory `test/libsimd/algorithms` with your unit test `.cpp` file.
8. Create a new folder for your puzzle benchmark program at directory `src`.
9. Simplify things: Copy `CMakeLists.txt` and `main.cpp` from an existing puzzle, so you speedup things a little bit.
10. Extend `CMakeLists.txt` at directory `src` with your new benchmark folder.
11. Simplify things again: Copy a bash script at directory `bench` to perform an automatic test suite and add the bash script to all available `Dockerfile`s at the end.

Many tiny steps to do to create a new puzzle. :smiley:

## License

Copyright (C) 2020 Franz Alt

This project is distributed under the [MIT License](https://opensource.org/licenses/MIT), see [LICENSE](./LICENSE) for more information.
