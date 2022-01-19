cmake_minimum_required(VERSION 3.21)
project(range-filtering)

message(STATUS "Configuring..." ${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -mpopcnt -pthread -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -Werror -mpopcnt -pthread -std=c++11")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

#enable_testing()

add_subdirectory(src)
#add_subdirectory(test)
add_subdirectory(bench)
add_subdirectory(external/SuRF)

add_executable(main main.cpp)
target_link_libraries(main TRIE)