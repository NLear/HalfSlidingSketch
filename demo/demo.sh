#!/bin/bash

cd ../src/Frequency/Sliding_Sketch/demo || exit
make clean
make
./main
make clean

cd ../../ECM/demo || exit
make clean
make
./main
make clean

cd ../../SWCM/demo || exit
make clean
make
./main
make clean

cd ../../../HeavyHitter/Sl_HK/demo/ || exit
make clean
make
./main
make clean

cd ../../lambda_Algorithm/demo || exit
make clean
make
./main
make clean

ulimit -s 1024000

cd ../../WCSS/demo || exit
make clean
make prec
make recall
./prec
./recall
make clean

cd ../../../Membership || exit
make clean
make
./main
make clean


