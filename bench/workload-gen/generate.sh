#!/bin/bash
#distributions=( "uniform" "normal" "powerlaw" )
distributions=( "powerlaw" )

for dist in "${distributions[@]}"
do
    echo "Generating files for ${dist} distribution..."
    python3 generator.py --alph-size=26 --alph-start=a --length-avg=15 --n=1000000 \
        --length-sigma=3 --letter-distribution="${dist}" --output="${dist}" --output-dir="1mln"
    echo "Done"
done