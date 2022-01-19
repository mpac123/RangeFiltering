add_executable(bench bench.cpp)
target_link_libraries(bench TRIE)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../external/SuRF/include")
