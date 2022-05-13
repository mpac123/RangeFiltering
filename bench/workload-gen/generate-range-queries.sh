#!/bin/bash
distributions=( "uniform" "normal" "powerlaw" "last_letter_different")
#distributions=( "powerlaw" )

for dist in "${distributions[@]}"
do
    echo "Generating files for ${dist} distribution..."
    python3 range-queries-generator.py --alph-size=26 --alph-start=a --length-avg=15 --n=50000 \
        --length-sigma=3 --letter-distribution="${dist}" --output="${dist}" --output-dir="100k_big" \
        --range-min-size=21 --range-max-size=25
    echo "Done"
done