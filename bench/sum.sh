#!/bin/bash

ROOT=$PWD

# check if GCC is installed
command -v gcc >/dev/null

if [[ "${?}" -ne 0 ]]; then
    GCC_INSTALLED=1
fi

# check if Clang is installed
command -v clang >/dev/null

if [[ "${?}" -ne 0 ]]; then
    CLANG_INSTALLED=1
fi

# check if Rscript is installed
command -v Rscript >/dev/null 2>&1 || { echo >&2 "Rscript is required but not installed. Aborting."; exit 1; }

if [ -z "$GCC_INSTALLED" ]; then
    echo "GCC installed"

    # create build directory for GCC
    mkdir -p $ROOT/build/gcc
    cd $ROOT/build/gcc

    # build benchmark program with optimization level O1
    cmake ../../../ -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DOPTIMIZATION_LEVEL=O1
    make -j`nproc`
    mv bench_sum $ROOT/bench_sum.gcc-O1

    rm -rf *

    # build benchmark program with optimization level O2
    cmake ../../../ -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DOPTIMIZATION_LEVEL=O2
    make -j`nproc`
    mv bench_sum $ROOT/bench_sum.gcc-O2

    rm -rf *

    # build benchmark program with optimization level O3
    cmake ../../../ -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DOPTIMIZATION_LEVEL=O3
    make -j`nproc`
    mv bench_sum $ROOT/bench_sum.gcc-O3

    cd $ROOT

    # start benchmark for optimization level O1
    ./bench_sum.gcc-O1 --csv-all
    mv sum_all.csv sum.gcc-O1_all.csv

    ./bench_sum.gcc-O2 --csv-all
    mv sum_all.csv sum.gcc-O2_all.csv

    ./bench_sum.gcc-O3 --csv-all
    mv sum_all.csv sum.gcc-O3_all.csv
fi

if [ -z "$CLANG_INSTALLED" ]; then
    echo "Clang installed"

    # create build directory for Clang
    mkdir -p $ROOT/build/clang
    cd $ROOT/build/clang

    # build benchmark program with optimization level O1
    cmake ../../../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOPTIMIZATION_LEVEL=O1
    make -j`nproc`
    mv bench_sum $ROOT/bench_sum.clang-O1

    rm -rf *

    # build benchmark program with optimization level O2
    cmake ../../../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOPTIMIZATION_LEVEL=O2
    make -j`nproc`
    mv bench_sum $ROOT/bench_sum.clang-O2

    rm -rf *

    # build benchmark program with optimization level O3
    cmake ../../../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOPTIMIZATION_LEVEL=O3
    make -j`nproc`
    mv bench_sum $ROOT/bench_sum.clang-O3

    cd $ROOT

    # start benchmark for optimization level O1
    ./bench_sum.clang-O1 --csv-all
    mv sum_all.csv sum.clang-O1_all.csv

    ./bench_sum.clang-O2 --csv-all
    mv sum_all.csv sum.clang-O2_all.csv

    ./bench_sum.clang-O3 --csv-all
    mv sum_all.csv sum.clang-O3_all.csv
fi

# create plots
Rscript multiplot.R
