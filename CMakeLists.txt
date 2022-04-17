cmake_minimum_required(VERSION 3.21)
project(range-filtering)

message(STATUS "Configuring..." ${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -mpopcnt -pthread -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -Werror -mpopcnt -pthread -std=c++17")

configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR} @ONLY)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/baseline/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/basic_filters/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/basic_filters/external/mmh")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/quotient-filter")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/range_filters/include")

enable_testing()

add_subdirectory(baseline)
add_subdirectory(bench)
add_subdirectory(external/SuRF)
add_subdirectory(basic_filters)
add_subdirectory(external/quotient-filter)
add_subdirectory(succinct_trie)
add_subdirectory(splash)
add_subdirectory(bloomed_splash)
add_subdirectory(rosetta)
add_subdirectory(range_filters)
add_subdirectory(bloomed_range_splash)

add_executable(main main.cpp)
target_link_libraries(main TRIE)