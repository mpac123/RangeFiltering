#!/bin/bash
distributions=( "uniform" "normal" "powerlaw" "last_letter_different" "increasing_degree")
#distributions=( "powerlaw" )

for dist in "${distributions[@]}"
do
    echo "Generating files for ${dist} distribution..."
    python3 generator.py --alph-size=26 --alph-start=a --length-avg=15 --n=110000 \
        --length-sigma=3 --letter-distribution="${dist}" --output="${dist}" --output-dir="110k_26_15_3"
    echo "Done"
done

# for dist in "${distributions[@]}"
# do
#     echo "Generating files for ${dist} distribution..."
#     python3 generator.py --alph-size=26 --alph-start=a --length-avg=10 --n=15 \
#         --length-sigma=2 --letter-distribution="${dist}" --output="${dist}" --output-dir="15_2"
#     echo "Done"
# done