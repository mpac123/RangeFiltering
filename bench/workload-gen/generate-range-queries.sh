#!/bin/bash
distributions=( "uniform" "normal" "powerlaw" "last_letter_different" )
#distributions=( "powerlaw" )

for dist in "${distributions[@]}"
do
    echo "Generating files for ${dist} distribution..."
    python3 range-queries-generator.py --alph-size=26 --alph-start=a --length-avg=15 --n=10000 \
        --length-sigma=0 --letter-distribution="${dist}" --output="${dist}" --output-dir="10k_26_15_0__4_8" \
        --range-min-size=4 --range-max-size=8
    echo "Done"
done 