#!/bin/bash

cd build
make CTEST_OUTPUT_ON_FAILURE=1 test
cd ..
