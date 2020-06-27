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

# check if sed is installed
command -v sed >/dev/null 2>&1 || { echo >&2 "sed is required but not installed. Aborting."; exit 1; }

# check if awk is installed
command -v awk >/dev/null 2>&1 || { echo >&2 "awk is required but not installed. Aborting."; exit 1; }

# determine CPU model name
CPU_MODEL_NAME=`LC_ALL=C lscpu | grep 'Model name' | cut -f 2 -d ":" | awk '{$1=$1}1' | sed -e 's/ /_/g'`

# create directory for all results

RESULT_DIR=$ROOT/results

mkdir -p $RESULT_DIR

if [ -z "$GCC_INSTALLED" ]; then
    echo "GCC installed"

    # create build directory for GCC
    mkdir -p $ROOT/build/gcc
    cd $ROOT/build/gcc

    # build benchmark program with optimization level O1
    cmake ../../../ -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DOPTIMIZATION_LEVEL=O1
    make -j`nproc`
    mv bench_base64 $RESULT_DIR/bench_base64.gcc-O1

    rm -rf *

    # build benchmark program with optimization level O2
    cmake ../../../ -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DOPTIMIZATION_LEVEL=O2
    make -j`nproc`
    mv bench_base64 $RESULT_DIR/bench_base64.gcc-O2

    rm -rf *

    # build benchmark program with optimization level O3
    cmake ../../../ -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DOPTIMIZATION_LEVEL=O3
    make -j`nproc`
    mv bench_base64 $RESULT_DIR/bench_base64.gcc-O3

    cd $RESULT_DIR

    # start benchmark for optimization level O1
    ./bench_base64.gcc-O1 --csv-all
    mv to_base64_all.csv "to_base64.$CPU_MODEL_NAME.gcc-O1_all.csv"
    mv from_base64_all.csv "from_base64.$CPU_MODEL_NAME.gcc-O1_all.csv"

    # start benchmark for optimization level O2
    ./bench_base64.gcc-O2 --csv-all
    mv to_base64_all.csv "to_base64.$CPU_MODEL_NAME.gcc-O2_all.csv"
    mv from_base64_all.csv "from_base64.$CPU_MODEL_NAME.gcc-O2_all.csv"

    # start benchmark for optimization level O3
    ./bench_base64.gcc-O3 --csv-all
    mv to_base64_all.csv "to_base64.$CPU_MODEL_NAME.gcc-O3_all.csv"
    mv from_base64_all.csv "from_base64.$CPU_MODEL_NAME.gcc-O3_all.csv"

fi

if [ -z "$CLANG_INSTALLED" ]; then
    echo "Clang installed"

    # create build directory for Clang
    mkdir -p $ROOT/build/clang
    cd $ROOT/build/clang

    # build benchmark program with optimization level O1
    cmake ../../../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOPTIMIZATION_LEVEL=O1
    make -j`nproc`
    mv bench_base64 $RESULT_DIR/bench_base64.clang-O1

    rm -rf *

    # build benchmark program with optimization level O2
    cmake ../../../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOPTIMIZATION_LEVEL=O2
    make -j`nproc`
    mv bench_base64 $RESULT_DIR/bench_base64.clang-O2

    rm -rf *

    # build benchmark program with optimization level O3
    cmake ../../../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOPTIMIZATION_LEVEL=O3
    make -j`nproc`
    mv bench_base64 $RESULT_DIR/bench_base64.clang-O3

    cd $RESULT_DIR

    # start benchmark for optimization level O1
    ./bench_base64.clang-O1 --csv-all
    mv to_base64_all.csv "to_base64.$CPU_MODEL_NAME.clang-O1_all.csv"
    mv from_base64_all.csv "from_base64.$CPU_MODEL_NAME.clang-O1_all.csv"

    # start benchmark for optimization level O2
    ./bench_base64.clang-O2 --csv-all
    mv to_base64_all.csv "to_base64.$CPU_MODEL_NAME.clang-O2_all.csv"
    mv from_base64_all.csv "from_base64.$CPU_MODEL_NAME.clang-O2_all.csv"

    # start benchmark for optimization level O3
    ./bench_base64.clang-O3 --csv-all
    mv to_base64_all.csv "to_base64.$CPU_MODEL_NAME.clang-O3_all.csv"
    mv from_base64_all.csv "from_base64.$CPU_MODEL_NAME.clang-O3_all.csv"

fi

# create plots
Rscript ../multiplot.R "Base64 Encoding" "to_base64.*_all.csv"
Rscript ../multiplot.R "Base64 Decoding" "from_base64.*_all.csv"
