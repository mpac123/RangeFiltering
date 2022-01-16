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