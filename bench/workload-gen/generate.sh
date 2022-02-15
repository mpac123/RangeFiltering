#!/bin/bash
distributions=( "uniform" "normal" "powerlaw" "last_letter_different")
#distributions=( "powerlaw" )

for dist in "${distributions[@]}"
do
    echo "Generating files for ${dist} distribution..."
    python3 generator.py --alph-size=26 --alph-start=a --length-avg=10 --n=15 \
        --length-sigma=2 --letter-distribution="${dist}" --output="${dist}" --output-dir="15"
    echo "Done"
done