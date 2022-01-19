# Range Filtering

## Install Dependencies
    sudo apt-get install build-essential cmake libgtest.dev
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp *.a /usr/lib

## Build
    mkdir build
    cd build
    cmake ..
    make -j

## Run Unit Tests
    make test

## Benchmark

### Run benchmark
```
    ./build/bench/bench
```

### Workloads
- Top 10000 English words come from [Google 10000 English Words repository](https://github.com/first20hours/google-10000-english)
- `words.txt` comes from [SuRF repository](https://github.com/efficient/SuRF/blob/master/test/words.txt)