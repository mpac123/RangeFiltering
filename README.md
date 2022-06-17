# Range Filtering

## Install Dependencies
    sudo apt-get install build-essential cmake libgtest.dev
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp *.a /usr/lib

## Build
    ./build.sh

## Run Unit Tests
    ./test.sh

## The project structure

### Relevant source code

The following directories contain the relevant source code:
- `basic_filters` contains implementation of Bloom filter,
- `chareq` contains implementation of CHaREQ (Compact Hash Table for Range Emptiness Queries)
- `rosetta` constains implementation of Rosetta and LilRosetta
- `splash` constains implementation of Splash

### Benchmarks

The `bench` folder contains C++ code running benchmarks on the data structures with different parameters, Python and bash scripts calling the C++ code, and more Python and bash scripts generating workloads with different query and data distributions.