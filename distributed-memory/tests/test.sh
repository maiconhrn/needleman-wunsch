#!/bin/sh

for i in $( seq 1 4 )
do
    for j in $( seq 0 1 )
    do
        ((time mpirun -np 4 ./build/nw tests/${i}0000_seq1.txt tests/${i}0000_seq2.txt \
        && echo "\n=============================\n") | tee -a tests/results/4_process/${i}0000.txt) \
    done
    echo "\n============fim==============\n"
done