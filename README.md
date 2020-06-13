# simd-puzzles

Algorithmic problems optimized with SIMD

## Motivation

During some software projects I had have to optimize different types of algorithms to improve their speed with SIMD techniques. Basically I've made a fallback version of an algorithm without SIMD. After that I've tried to write a SIMD version of the algorithm and push this version as fast as I can. In most cases, I did this without being able to compare different SIMD implementations of an algorithm because there was no suitable test environment for different implementation approaches.

With this project I would like to collect some algorithmic SIMD puzzles and compare these on different architectures and compilers. Furthermore I hope some other guys [contributing](./CONTRIBUTING.md) algorithms puzzles and optimize these with SIMD. :smiley:

## Puzzles

| Algorithm | AVX2 | AVX-512 | NEON |
|-----------|------|---------|------|
| [Sum Bytes](./doc/sum/sum.md) | :heavy_check_mark: | :x: | :x: |

## Planned

* Further more puzzles (Base64, ...)
* AVX-512 and NEON implementations
* Compare multithreaded executions
* ...

## Directory Structure

The main project directory structure is figured in the following tree.

    .
    ├── bench
    │   └── <puzzle benchmark scripts>
    ├── cmake
    ├── doc
    │   └── <puzzle docs>
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

To build the library on Unix/Linux try:

    mkdir build
    cd build
    cmake ..
    make -j`nproc`

## Extend Puzzles

TODO

## License

Copyright (C) 2020 Franz Alt

This project is distributed under the [MIT License](https://opensource.org/licenses/MIT), see [LICENSE](./LICENSE) for more information.
