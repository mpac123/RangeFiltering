#!/bin/bash
echo "Generating files distribution..."
python3 integer-set-generator.py --n=500 --u=1000 \
    --output-dir="1e4"
echo "Done"