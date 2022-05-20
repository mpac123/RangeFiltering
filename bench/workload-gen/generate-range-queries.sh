#!/bin/bash
distributions=( "uniform" "normal" "powerlaw" "last_letter_different" "increasing_degree" )
#distributions=( "powerlaw" )

for dist in "${distributions[@]}"
do
    echo "Generating files for ${dist} distribution..."
    python3 range-queries-generator.py --alph-size=26 --alph-start=a --length-avg=15 --n=100000 \
        --length-sigma=3 --letter-distribution="${dist}" --output="${dist}" --output-dir="100k_26_15_3__2_100" \
        --range-min-size=2 --range-max-size=100
    echo "Done"
done