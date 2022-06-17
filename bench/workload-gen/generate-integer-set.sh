#!/bin/bash
echo "Generating files distribution..."
python3 integer-set-generator.py --n=500 --u=100000 \
    --output-dir="1e5"
echo "Done"