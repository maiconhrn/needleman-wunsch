#!/bin/sh

for i in $( seq 1 4 )
do
    for j in $( seq 0 4 )
    do
        (echo "Sequential" \
        && build/nw tests/${i}0000_seq1.txt tests/${i}0000_seq2.txt \
        && echo "\n=============================\n") | tee -a tests/results/sequential/${i}0000.txt
    done
done