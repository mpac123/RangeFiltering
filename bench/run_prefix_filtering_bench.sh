#!/bin/bash
distributions=( "uniform" "normal" "powerlaw")
querytype=("similar" "random" "last_letter")
dir="100k"

# Params for 100k
BF_min=1000000
BF_max=25000000
BF_interval=1000000
QF_start_q=20
QF_end_q=20
QF_start_r=2
QF_end_r=16

# Params for 100
if [ "$dir" = "100" ]; then
    BF_min=1000
    BF_max=20000
    BF_interval=1000
    QF_start_q=11
    QF_end_q=11
    QF_start_r=1
    QF_end_r=8
fi

# Params for 1mln
if [ "$dir" = "1mln" ]; then
    BF_min=50000000
    BF_max=160000000
    BF_interval=20000000
    QF_start_q=22
    QF_end_q=22
    QF_start_r=4
    QF_end_r=4
fi

mkdir -p results/${dir}

for dist in "${distributions[@]}"
do
    for qt in "${querytype[@]}"
    do
        echo "${dist}_${qt}"
        cd ..
        build/bench/prefix_filtering_bench $dist $qt "bench/workload-gen/workloads/${dir}/" \
            $BF_min $BF_max $BF_interval $QF_start_q $QF_end_q $QF_start_r $QF_end_r > bench/results/${dir}/${dist}_${qt}.txt
        cd bench
    done
done